#include "Model.h"

#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

#include <glm/gtx/quaternion.hpp>
#include <glm/glm.hpp>

#include "AssetManager.h"
#include "Project.h"

#include "Log.h"

namespace Engine
{
Model::Model(std::filesystem::path path, std::filesystem::path dst, const bool flipWindingOrder,
             const bool loadMaterial)
    : m_Path(path), m_Dst(dst)
{
    // auto fullPath = Utils::Path::GetAbsolute(std::string(path));
    if (!LoadModel(path, flipWindingOrder, loadMaterial))
    {
        LOG_CORE_ERROR("Failed to load model: {0}", path.string());
        return;
    }
}

Model::Model(const std::string &name, const std::vector<Vertex> &vertices, std::vector<unsigned int> &indices,
             AssetHandle materialHandle) noexcept
{
    m_Meshes.emplace_back(name, vertices, indices, materialHandle);
}

Model::Model(const Mesh &mesh) noexcept { m_Meshes.push_back(mesh); }

void Model::AttachMesh(const Mesh mesh) noexcept { m_Meshes.push_back(mesh); }

void Model::Delete()
{
    for (auto &mesh : m_Meshes) mesh.Clear();
}

bool Model::LoadModel(const std::filesystem::path &path, const bool flipWindingOrder, const bool loadMaterial)
{
    Assimp::Importer importer;
    const aiScene *scene = nullptr;
    auto importFlags = aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FixInfacingNormals |
                       aiProcess_CalcTangentSpace | aiProcess_OptimizeGraph;

    if (flipWindingOrder)
        scene = importer.ReadFile(path.string().c_str(), importFlags |= aiProcess_FlipWindingOrder);
    else
        scene = importer.ReadFile(path.string().c_str(), importFlags);

    // Check if scene is not null and model is done loading
    if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        LOG_CORE_ERROR("Assimp Error for {0}: {1}", m_Path, importer.GetErrorString());
        importer.FreeScene();

        return false;
    }
	if (scene->HasAnimations())
	{
        m_HasAnimations = true;
        return true;
	}

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
    for (auto i = 0; i < node->mNumChildren; i++) ProcessNode(node->mChildren[i], scene, loadMaterial);
}

Mesh Model::ProcessMesh(aiMesh *mesh, const aiScene *scene, bool loadMaterial)
{
    std::vector<Vertex> vertices;

    for (auto i = 0; i < mesh->mNumVertices; ++i)
    {
        Vertex vertex{};

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
        if (mesh->mTangents)
        {
            vertex.Tangent.x = mesh->mTangents[i].x;
            vertex.Tangent.y = mesh->mTangents[i].y;
            vertex.Tangent.z = mesh->mTangents[i].z;
        }
        if (mesh->mBitangents)
        {
            vertex.Bitangent.x = mesh->mBitangents[i].x;
            vertex.Bitangent.y = mesh->mBitangents[i].y;
            vertex.Bitangent.z = mesh->mBitangents[i].z;
        }

        vertex.TexCoords = glm::vec2(0.0f);
        if (mesh->HasTextureCoords(0) && loadMaterial)
        {
            // Just take the first set of texture coords (since we could have up to 8)
            vertex.TexCoords.x = mesh->mTextureCoords[0][i].x;
            vertex.TexCoords.y = mesh->mTextureCoords[0][i].y;
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

            std::string materialName = name.C_Str();

            std::array<aiString, 5> materialPaths;

            mat->GetTexture(AI_MATKEY_BASE_COLOR_TEXTURE, &materialPaths[ParameterType::ALBEDO]);
            mat->GetTexture(aiTextureType_NORMALS, 0, &materialPaths[ParameterType::NORMAL]);
            mat->GetTexture(AI_MATKEY_METALLIC_TEXTURE, &materialPaths[ParameterType::METALLIC]);
            mat->GetTexture(AI_MATKEY_ROUGHNESS_TEXTURE, &materialPaths[ParameterType::ROUGHNESS]);
            mat->GetTexture(aiTextureType_LIGHTMAP, 0, &materialPaths[ParameterType::AO]);

            MaterialRef material = std::make_shared<Material>();
            material->Init(materialName, materialPaths[ParameterType::ALBEDO].C_Str(),
                           materialPaths[ParameterType::NORMAL].C_Str(), materialPaths[ParameterType::METALLIC].C_Str(),
                           materialPaths[ParameterType::ROUGHNESS].C_Str(), materialPaths[ParameterType::AO].C_Str());
            material->IsDefault = true;

            std::string matName = material->Name.empty()
                                      ? (m_Path.stem().string() + "_mat" + std::to_string(mesh->mMaterialIndex))
                                      : material->Name;
            material->Name = matName;

            /*auto relativePath = std::filesystem::relative(m_Dst / m_Path.stem(), Project::GetAssetDirectory());
            material->Handle = AssetManager::AddAsset(material, relativePath / (matName + ".material"));
            auto defaultMatPath = relativePath / (matName + ".material");*/

            ++m_NumOfMaterials;
            return {mesh->mName.C_Str(), vertices, indices, material};
        }
    }

    return {mesh->mName.C_Str(), vertices, indices, nullptr};
}
} // namespace Engine
