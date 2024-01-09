#include "Animation.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

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

Animation::Animation(const std::filesystem::path &animationPath, SkinnedModel *model) 
{
    LoadAnimation(animationPath, model, 0);
}

void Animation::LoadAnimation(const std::filesystem::path &animationPath, SkinnedModel *model, int index) 
{
    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(animationPath.string().c_str(), aiProcess_Triangulate);
    assert(scene && scene->mRootNode);
    // we can have more than one animation sequence?
    auto animation = scene->mAnimations[index];
    m_Duration = animation->mDuration;
    m_TicksPerSecond = animation->mTicksPerSecond;

    ReadHierarchyData(m_RootNode, scene->mRootNode);
    ReadMissingBones(animation, *model);
}

Bone *Animation::FindBone(const std::string &name)
{
	auto iter = std::find_if(m_Bones.begin(), m_Bones.end(), [&](const Bone& bone) {
		return bone.GetBoneName() == name;
	});
    if (iter == m_Bones.end()) return nullptr;
    else return &(*iter);
}

void Animation::ReadMissingBones(const aiAnimation *animation, SkinnedModel &model)
{
	int size = animation->mNumChannels;

	auto &boneInfoMap = model.GetBoneInfoMap();
	int &boneCount = model.GetBoneCount();

	// reading channels (bones engaged in an animation and their keyframes)
	for (int i = 0; i < size; i++)
	{
		auto channel = animation->mChannels[i];
		std::string boneName = channel->mNodeName.data;

		if (boneInfoMap.find(boneName) == boneInfoMap.end())
		{
			boneInfoMap[boneName].ID = boneCount;
			boneCount++;
		}
		m_Bones.push_back({channel->mNodeName.data, boneInfoMap[channel->mNodeName.data].ID, channel});
	}
}

void Animation::ReadHierarchyData(AssimpNodeData &dest, const aiNode *src) 
{
    assert(src);

    dest.Name = src->mName.data;
    dest.Transformation = ConvertMatrixToGLMFormat(src->mTransformation);
    dest.ChildrenCount = src->mNumChildren;

    for (int i = 0; i < src->mNumChildren; i++)
    {
        AssimpNodeData newData;
        ReadHierarchyData(newData, src->mChildren[i]);
        dest.Children.push_back(newData);
    }
}
} // namespace Engine
