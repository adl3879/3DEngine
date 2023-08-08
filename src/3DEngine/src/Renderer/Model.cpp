#include "Model.h"

#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

#include <glm/gtx/quaternion.hpp>
#include <glm/glm.hpp>

#include "ResourceManager.h"
#include "PlatformUtils.h"
#include "AssetManager.h"
#include "Project.h"

#include "Log.h"

namespace Engine
{
Model::Model(const std::filesystem::path &path, const bool flipWindingOrder, const bool loadMaterial) : m_Path(path)
{
    // auto fullPath = Utils::Path::GetAbsolute(std::string(path));
    if (!LoadModel(path, flipWindingOrder, loadMaterial)) LOG_CORE_ERROR("Failed to load model: {0}", path.string());
}

Model::Model(const std::vector<Vertex> &vertices, std::vector<unsigned int> &indices,
             const MaterialRef &material) noexcept
{
    m_Meshes.emplace_back(vertices, indices, material);
}

Model::Model(const Mesh &mesh) noexcept { m_Meshes.push_back(mesh); }

void Model::AttachMesh(const Mesh mesh) noexcept { m_Meshes.push_back(mesh); }

void Model::Delete()
{
    for (auto &mesh : m_Meshes) mesh.VAO.Delete();
}

bool Model::LoadModel(const std::filesystem::path &path, const bool flipWindingOrder, const bool loadMaterial)
{
    Assimp::Importer importer;
    const aiScene *scene = nullptr;

    if (flipWindingOrder)
    {
        scene = importer.ReadFile(
            path.c_str(), aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_GenUVCoords |
                              aiProcess_SortByPType | aiProcess_RemoveRedundantMaterials | aiProcess_FindInvalidData |
                              aiProcess_FlipUVs | aiProcess_FlipWindingOrder | // Reverse back-face culling
                              aiProcess_CalcTangentSpace | aiProcess_OptimizeMeshes | aiProcess_SplitLargeMeshes |
                              aiProcess_GenBoundingBoxes);
    }
    else
    {
        scene = importer.ReadFile(
            path.c_str(), aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_GenUVCoords |
                              aiProcess_SortByPType | aiProcess_RemoveRedundantMaterials | aiProcess_FindInvalidData |
                              aiProcess_FlipUVs | aiProcess_CalcTangentSpace | aiProcess_GenSmoothNormals |
                              aiProcess_ImproveCacheLocality | aiProcess_OptimizeMeshes | aiProcess_SplitLargeMeshes |
                              aiProcess_GenBoundingBoxes);
    }

    // Check if scene is not null and model is done loading
    if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        LOG_CORE_ERROR("Assimp Error for {0}: {1}", m_Path, importer.GetErrorString());
        importer.FreeScene();

        return false;
    }

    m_Path = path.string().substr(0, path.string().find_last_of('/'));
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
    }

    // Process their children via recursive tree traversal
    for (auto i = 0; i < node->mNumChildren; ++i) ProcessNode(node->mChildren[i], scene, loadMaterial);
}

Mesh Model::ProcessMesh(aiMesh *mesh, const aiScene *scene, const bool loadMaterial)
{
    std::vector<Vertex> vertices;
    VertexSOA vertexSOA;

    for (auto i = 0; i < mesh->mNumVertices; ++i)
    {
        vertexSOA.Colors.emplace_back(glm::vec4(1.0f));
        vertexSOA.EntityIDs.emplace_back(-1.0f);
        Vertex vertex;

        if (mesh->HasPositions())
        {
            vertex.Position.x = mesh->mVertices[i].x;
            vertex.Position.y = mesh->mVertices[i].y;
            vertex.Position.z = mesh->mVertices[i].z;
            vertexSOA.Positions.emplace_back(vertex.Position);
        }

        if (mesh->HasNormals())
        {
            vertex.Normal.x = mesh->mNormals[i].x;
            vertex.Normal.y = mesh->mNormals[i].y;
            vertex.Normal.z = mesh->mNormals[i].z;
            vertexSOA.Normals.emplace_back(vertex.Normal);
        }

        if (mesh->HasTextureCoords(0) && loadMaterial)
        {
            // Just take the first set of texture coords (since we could have up to 8)
            vertex.TexCoords.x = mesh->mTextureCoords[0][i].x;
            vertex.TexCoords.y = mesh->mTextureCoords[0][i].y;
            vertexSOA.TexCoords.emplace_back(vertex.TexCoords);
        }
        else
        {
            vertex.TexCoords = glm::vec2(0.0f);
            vertexSOA.TexCoords.emplace_back(vertex.TexCoords);
        }

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
            m_DefaultMaterial = name.C_Str();

            aiString albedoPath, metallicPath, normalPath, roughnessPath, alphaMaskPath;
            std::array<aiString, 5> materialPaths;

            mat->GetTexture(AI_MATKEY_BASE_COLOR_TEXTURE, &materialPaths[0]);
            mat->GetTexture(AI_MATKEY_METALLIC_TEXTURE, &materialPaths[1]);
            mat->GetTexture(AI_MATKEY_ROUGHNESS_TEXTURE, &materialPaths[3]);

            AssetHandle materialHandles[5];

            // TODO: This is a bit of a hack, make it more performant
            for (auto i = 0; i < materialPaths.size(); ++i)
            {
                auto handle = AssetManager::GetAssetHandleFromPath(GetRelativeTexturePath(materialPaths[i]));
                if (handle > 0)
                    materialHandles[i] = handle;
                else
                    materialHandles[i] = AssetManager::ImportAsset(GetRelativeTexturePath(materialPaths[i]));
            }

            MaterialRef material = std::make_shared<Material>();
            material->Init(name.C_Str(), materialHandles[0], materialHandles[1], materialHandles[2], materialHandles[3],
                           materialHandles[4]);

            ++m_NumOfMaterials;
            return Mesh(vertexSOA, indices, material);
        }
    }

    return Mesh(vertexSOA, indices);
}

std::filesystem::path Model::GetRelativeTexturePath(const aiString &path) const
{
    if (path.C_Str()[0] != '\0')
        return std::filesystem::relative(m_Path + path.C_Str(), Project::GetAssetDirectory());
    else
        return std::filesystem::path();
}
} // namespace Engine