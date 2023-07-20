#include "Model.h"

#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

#include <glm/gtx/quaternion.hpp>
#include <glm/glm.hpp>

#include "ResourceManager.h"
#include "PlatformUtils.h"

#include "Log.h"

namespace Engine
{
Model::Model(int entityID, const std::string_view path, const std::string_view name, const bool flipWindingOrder,
             const bool loadMaterial)
    : m_EntityID(entityID), m_Path(path), m_Name(name)
{
    auto fullPath = Utils::Path::GetAbsolute(std::string(path));
    if (!LoadModel(fullPath, flipWindingOrder, loadMaterial)) LOG_CORE_ERROR("Failed to load model: {0}", name);
}

Model::Model(int entityID, const std::string_view name, const std::vector<Vertex> &vertices,
             std::vector<unsigned int> &indices, const MaterialPtr &material) noexcept
    : m_EntityID(entityID), m_Name(name)
{
    m_Meshes.emplace_back(vertices, indices, material);
}

Model::Model(int entityID, const std::string_view name, const Mesh &mesh) noexcept : m_EntityID(entityID), m_Name(name)
{
    m_Meshes.push_back(mesh);
}

void Model::AttachMesh(const Mesh mesh) noexcept { m_Meshes.push_back(mesh); }

void Model::Delete()
{
    for (auto &mesh : m_Meshes) mesh.VAO.Delete();
}

bool Model::LoadModel(const std::string_view path, const bool flipWindingOrder, const bool loadMaterial)
{
    Assimp::Importer importer;
    const aiScene *scene = nullptr;

    if (flipWindingOrder)
    {
        scene = importer.ReadFile(
            path.data(), aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_GenUVCoords |
                             aiProcess_SortByPType | aiProcess_RemoveRedundantMaterials | aiProcess_FindInvalidData |
                             aiProcess_FlipUVs | aiProcess_FlipWindingOrder | // Reverse back-face culling
                             aiProcess_CalcTangentSpace | aiProcess_OptimizeMeshes | aiProcess_SplitLargeMeshes |
                             aiProcess_GenBoundingBoxes);
    }
    else
    {
        scene = importer.ReadFile(
            path.data(), aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_GenUVCoords |
                             aiProcess_SortByPType | aiProcess_RemoveRedundantMaterials | aiProcess_FindInvalidData |
                             aiProcess_FlipUVs | aiProcess_CalcTangentSpace | aiProcess_GenSmoothNormals |
                             aiProcess_ImproveCacheLocality | aiProcess_OptimizeMeshes | aiProcess_SplitLargeMeshes |
                             aiProcess_GenBoundingBoxes);
    }

    // Check if scene is not null and model is done loading
    if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        LOG_CORE_ERROR("Assimp Error for {0}: {1}", m_Name, importer.GetErrorString());
        importer.FreeScene();

        return false;
    }

    m_Path = path.substr(0, path.find_last_of('/')); // Strip the model file name and keep the model folder.
    m_Path += "/";

    ProcessNode(scene->mRootNode, scene, loadMaterial);

    importer.FreeScene();
    return true;
}

void Model::ProcessNode(aiNode *node, const aiScene *scene, const bool loadMaterial)
{
    // Process all node meshes
    for (auto i = 0; i < node->mNumMeshes; ++i)
    {
        auto *mesh = scene->mMeshes[node->mMeshes[i]];
        m_Meshes.push_back(ProcessMesh(mesh, scene, loadMaterial));

        // Add the bounding box for the mesh
        const aiAABB &box = mesh->mAABB;
        m_BoundingBoxes.push_back(
            {glm::vec3(box.mMin.x, box.mMin.y, box.mMin.z), glm::vec3(box.mMax.x, box.mMax.y, box.mMax.z)});
    }

    // Process their children via recursive tree traversal
    for (auto i = 0; i < node->mNumChildren; ++i) ProcessNode(node->mChildren[i], scene, loadMaterial);
}

Mesh Model::ProcessMesh(aiMesh *mesh, const aiScene *scene, const bool loadMaterial)
{

    std::vector<Vertex> vertices;

    for (auto i = 0; i < mesh->mNumVertices; ++i)
    {
        Vertex vertex;

        if (mesh->HasPositions())
        {
            vertex.Position.x = mesh->mVertices[i].x;
            vertex.Position.y = mesh->mVertices[i].y;
            vertex.Position.z = mesh->mVertices[i].z;
        }

        if (mesh->HasNormals())
        {
            vertex.Normal.x = mesh->mNormals[i].x;
            vertex.Normal.y = mesh->mNormals[i].y;
            vertex.Normal.z = mesh->mNormals[i].z;
        }

        // if (mesh->HasTangentsAndBitangents())
        // {
        //     vertex.Tangent.x = mesh->mTangents[i].x;
        //     vertex.Tangent.y = mesh->mTangents[i].y;
        //     vertex.Tangent.z = mesh->mTangents[i].z;
        // }

        if (mesh->HasTextureCoords(0) && loadMaterial)
        {
            // Just take the first set of texture coords (since we could have up to 8)
            vertex.TexCoords.x = mesh->mTextureCoords[0][i].x;
            vertex.TexCoords.y = mesh->mTextureCoords[0][i].y;
        }
        else
        {
            vertex.TexCoords = glm::vec2(0.0f);
        }

        vertex.EditorID = m_EntityID + 1;

        vertices.push_back(vertex);
    }

    // Get indices from each face
    std::vector<unsigned int> indices;
    for (auto i = 0; i < mesh->mNumFaces; ++i)
    {
        const auto face = mesh->mFaces[i];
        for (auto j = 0; j < face.mNumIndices; ++j)
        {
            indices.emplace_back(face.mIndices[j]);
        }
    }

    // Process material
    // http://assimp.sourceforge.net/lib_html/structai_material.html
    if (loadMaterial)
    {
        if (mesh->mMaterialIndex >= 0)
        {
            const auto *mat = scene->mMaterials[mesh->mMaterialIndex];

            aiString name;
            mat->Get(AI_MATKEY_NAME, name);

            // Is the material cached?
            const auto cachedMaterial = ResourceManager::Instance().GetMaterial(name.C_Str());
            if (cachedMaterial.has_value())
            {
                return Mesh(vertices, indices, cachedMaterial.value());
            }

            // Get the first texture for each texture type we need
            // since there could be multiple textures per type
            aiString albedoPath, metallicPath, normalPath, roughnessPath, alphaMaskPath;

            mat->GetTexture(AI_MATKEY_BASE_COLOR_TEXTURE, &albedoPath);
            mat->GetTexture(AI_MATKEY_METALLIC_TEXTURE, &metallicPath);
            mat->GetTexture(AI_MATKEY_ROUGHNESS_TEXTURE, &roughnessPath);

            const auto newMaterial = ResourceManager::Instance().CacheMaterial(
                name.C_Str(), m_Path + albedoPath.C_Str(), "", m_Path + metallicPath.C_Str(),
                m_Path + normalPath.C_Str(), m_Path + roughnessPath.C_Str(), m_Path + alphaMaskPath.C_Str());

            ++m_NumOfMaterials;
            return Mesh(vertices, indices, newMaterial);
        }
    }

    return Mesh(vertices, indices);
}
} // namespace Engine