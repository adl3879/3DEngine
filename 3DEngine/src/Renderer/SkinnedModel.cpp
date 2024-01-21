#include "SkinnedModel.h"

#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

#include "Log.h"
#include "AssetManager.h"
#include "Project.h"

namespace Engine
{
static inline glm::mat4 ConvertMatrixToGLMFormat(const aiMatrix4x4 &from)
{
    glm::mat4 to;
    // the a,b,c,d in assimp is the row ; the 1,2,3,4 is the column
    to[0][0] = from.a1; to[1][0] = from.a2;
    to[2][0] = from.a3; to[3][0] = from.a4;
    to[0][1] = from.b1; to[1][1] = from.b2;
    to[2][1] = from.b3; to[3][1] = from.b4;
    to[0][2] = from.c1; to[1][2] = from.c2;
    to[2][2] = from.c3; to[3][2] = from.c4;
    to[0][3] = from.d1; to[1][3] = from.d2;
    to[2][3] = from.d3; to[3][3] = from.d4;
    return to;
}

SkinnedModel::SkinnedModel(const std::filesystem::path &path)
	: m_Path(path)
{ 
	if (!LoadModel(path)) LOG_CORE_ERROR("Failed to load {}", path.string());
}

bool SkinnedModel::LoadModel(const std::filesystem::path &path)
{
    Assimp::Importer importer;
    const aiScene *scene = nullptr;
    auto importFlags = aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace;
   
    scene = importer.ReadFile(path.string().c_str(), importFlags);

    // Check if scene is not null and model is done loading
    if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        LOG_CORE_ERROR("Assimp Error for {0}: {1}", path, importer.GetErrorString());
        importer.FreeScene();

        return false;
    }

	if (scene->HasAnimations()) m_NumAnims = scene->mNumAnimations;

    ProcessNode(scene->mRootNode, scene);

    importer.FreeScene();
}

void SkinnedModel::ProcessNode(aiNode *node, const aiScene *scene) 
{
    // Process all node meshes
    for (auto i = 0; i < node->mNumMeshes; ++i)
    {
        auto *mesh = scene->mMeshes[node->mMeshes[i]];
        m_Meshes.push_back(ProcessMesh(mesh, scene));
    }

    // Process their children via recursive tree traversal
    for (auto i = 0; i < node->mNumChildren; ++i) ProcessNode(node->mChildren[i], scene);
}

void SkinnedModel::SetVertexBoneDataToDefault(SkinnedVertex &vertex) 
{
	for (int i = 0; i < MAX_BONE_INFLUENCE; i++)
	{
        vertex.BoneIDs[i] = -1;
        vertex.Weights[i] = 0.0f;
	}
}

void SkinnedModel::SetVertexBoneData(SkinnedVertex &vertex, int boneId, float weight) 
{
    for (int i = 0; i < MAX_BONE_INFLUENCE; ++i)
    {
		if (vertex.BoneIDs[i] == boneId) return;
    }

    if (weight == 0.0f) return;

	for (int i = 0; i < MAX_BONE_INFLUENCE; ++i)
	{
		if (vertex.BoneIDs[i] < 0)
		{
			vertex.Weights[i] = weight;
			vertex.BoneIDs[i] = boneId;
			break;
		}
	}
}

SkinnedMesh SkinnedModel::ProcessMesh(aiMesh *mesh, const aiScene *scene) 
{ 
	std::vector<SkinnedVertex> vertices;
	std::vector<uint32_t> indices;

	for (auto i = 0; i < mesh->mNumVertices; ++i)
    {
        SkinnedVertex vertex{};
		SetVertexBoneDataToDefault(vertex);

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
        if (mesh->HasTextureCoords(0))
        {
            // Just take the first set of texture coords (since we could have up to 8)
            vertex.TexCoords.x = mesh->mTextureCoords[0][i].x;
            vertex.TexCoords.y = mesh->mTextureCoords[0][i].y;
        }

        vertices.push_back(vertex);
    }

    for (auto i = 0; i < mesh->mNumFaces; ++i)
    {
        const auto face = mesh->mFaces[i];
        for (auto j = 0; j < face.mNumIndices; ++j)
        {
			indices.emplace_back(face.mIndices[j]);
        }
    }

	ExtractBoneWeightForVertices(vertices, mesh, scene);

	 if (mesh->mMaterialIndex >= 0)
     {
		const auto *mat = scene->mMaterials[mesh->mMaterialIndex];

		aiString name;
        mat->Get(AI_MATKEY_NAME, name);

        std::string materialName = name.C_Str();

        std::array<aiString, 5> materialPaths;

        mat->GetTexture(aiTextureType_DIFFUSE, 0, &materialPaths[ParameterType::ALBEDO]);
        mat->GetTexture(aiTextureType_NORMALS, 0, &materialPaths[ParameterType::NORMAL]);
        mat->GetTexture(aiTextureType_METALNESS, 0, &materialPaths[ParameterType::METALLIC]);
        mat->GetTexture(aiTextureType_DIFFUSE_ROUGHNESS, 0, &materialPaths[ParameterType::ROUGHNESS]);
        mat->GetTexture(aiTextureType_AMBIENT_OCCLUSION, 0, &materialPaths[ParameterType::AO]);

        MaterialRef material = std::make_shared<Material>();
        material->Init(materialName, materialPaths[ParameterType::ALBEDO].C_Str(),
                       materialPaths[ParameterType::NORMAL].C_Str(), materialPaths[ParameterType::METALLIC].C_Str(),
                       materialPaths[ParameterType::ROUGHNESS].C_Str(), materialPaths[ParameterType::AO].C_Str());
        material->IsDefault = true;

        std::string matName = material->Name.empty()
                                  ? (m_Path.stem().string() + "_mat" + std::to_string(mesh->mMaterialIndex))
                                  : material->Name;
        material->Name = matName;

        return {mesh->mName.C_Str(), vertices, indices, material};
     }

	 return {mesh->mName.C_Str(), vertices, indices, nullptr};
}

void SkinnedModel::ExtractBoneWeightForVertices(std::vector<SkinnedVertex> &vertices, aiMesh *mesh, const aiScene *scene)
{
	for (int boneIndex = 0; boneIndex < mesh->mNumBones; boneIndex++)
	{
        int boneID = -1;
        std::string boneName = mesh->mBones[boneIndex]->mName.C_Str();

        if (m_BoneInfoMap.find(boneName) == m_BoneInfoMap.end())
        {
            BoneInfo newBoneInfo;
            newBoneInfo.ID = m_BoneCounter;
            newBoneInfo.Offset = ConvertMatrixToGLMFormat(mesh->mBones[boneIndex]->mOffsetMatrix);

            m_BoneInfoMap[boneName] = newBoneInfo;
            boneID = m_BoneCounter;
            m_BoneCounter++;
        }
        else
        {
			boneID = m_BoneInfoMap[boneName].ID;
        }
        assert(boneID != -1);

        auto weights = mesh->mBones[boneIndex]->mWeights;
        int numWeights = mesh->mBones[boneIndex]->mNumWeights;

        for (int weightIndex = 0; weightIndex < numWeights; ++weightIndex)
        {
            int vertexId = weights[weightIndex].mVertexId;
            float weight = weights[weightIndex].mWeight;
            assert(vertexId <= vertices.size());
            SetVertexBoneData(vertices[vertexId], boneID, weight);
        }
	}
}
} // namespace Engine
