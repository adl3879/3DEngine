#pragma once

#include <filesystem>
#include <glm/glm.hpp>
#include <map>
#include <memory>
#include <vector>

#include "Vertex.h"
#include "SkinnedMesh.h"
#include "Asset.h"

struct aiScene;
struct aiNode;
struct aiMesh;
struct aiString;

namespace Engine
{
struct BoneInfo
{
    int ID;
    glm::mat4 Offset;
};

class SkinnedModel : public Asset
{
  public:
    SkinnedModel(const std::filesystem::path &path, const std::filesystem::path &dst);

	auto &GetBoneInfoMap() { return m_BoneInfoMap; }
    int &GetBoneCount() { return m_BoneCounter; }
    int GetNumAnimations() const { return m_NumAnims; }

	[[nodiscard]] AssetType GetType() const override { return AssetType::SkinnedMesh; }

  private:
    bool LoadModel(const std::filesystem::path &path);
    void ProcessNode(aiNode *node, const aiScene *scene);
    void SetVertexBoneDataToDefault(SkinnedVertex &vertex);
    void SetVertexBoneData(SkinnedVertex &vertex, int boneId, float weight);
    SkinnedMesh ProcessMesh(aiMesh *mesh, const aiScene *scene);
    void ExtractBoneWeightForVertices(std::vector<SkinnedVertex> &vertices, aiMesh *mesh, const aiScene *scene);

  private:
    std::vector<SkinnedMesh> m_Meshes;
    std::map<std::string, BoneInfo> m_BoneInfoMap;
    int m_BoneCounter = 0;
    int m_NumAnims = 0;

	std::filesystem::path m_Path, m_Dst;
};

using SkinnedModelRef = std::shared_ptr<SkinnedModel>;
}
