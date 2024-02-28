#pragma once

#include <filesystem>
#include <glm/glm.hpp>

#include "SkinnedModel.h"
#include "Bone.h"

struct aiAnimation;

namespace Engine
{
struct AssimpNodeData
{
    glm::mat4 Transformation;
    std::string Name;
    int ChildrenCount;
    std::vector<AssimpNodeData> Children;
};

class Animation
{
  public:
    Animation() = default;
    Animation(const std::filesystem::path &animationPath, SkinnedMeshData *meshData);
    ~Animation() = default;

	void LoadAnimation(const std::filesystem::path &animationPath, SkinnedMeshData *meshData, int index);
	Bone *FindBone(const std::string &name);

	inline float GetTicksPerSecond() { return m_TicksPerSecond; }
    inline float GetDuration() { return m_Duration; }
    inline const AssimpNodeData &GetRootNode() { return m_RootNode; }
    inline const std::map<std::string, BoneInfo> &GetBoneIDMap() { return m_BoneInfoMap; }
	inline const std::string &GetName() { return m_Name; }

  private:
    void ReadMissingBones(const aiAnimation *animation, SkinnedMeshData meshData);
    void ReadHierarchyData(AssimpNodeData &dest, const aiNode *src);

  private:
    float m_Duration;
    int m_TicksPerSecond;
    std::vector<Bone> m_Bones;
    AssimpNodeData m_RootNode;
    std::map<std::string, BoneInfo> m_BoneInfoMap;
	std::string m_Name;
};
}
