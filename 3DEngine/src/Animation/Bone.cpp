#include "Bone.h"

#include <glm/gtc/matrix_transform.hpp>
#include <assimp/anim.h>

namespace Engine
{
Bone::Bone(const std::string &name, int id, const aiNodeAnim *channel) 
	: m_Name(name), m_ID(id), m_LocalTransform(1.0f)
{
    m_NumPositions = channel->mNumPositionKeys;
	for (int positionIndex = 0; positionIndex < m_NumPositions; ++positionIndex)
	{
		aiVector3D aiPosition = channel->mPositionKeys[positionIndex].mValue;
		float timeStamp = channel->mPositionKeys[positionIndex].mTime;
		KeyPosition data;
		data.Position = {aiPosition.x, aiPosition.y, aiPosition.z};
		data.Timestamp = timeStamp;
        m_Positions.push_back(data);
	}

	m_NumRotations = channel->mNumRotationKeys;
    for (int rotationIndex = 0; rotationIndex < m_NumRotations; ++rotationIndex)
    {
		aiQuaternion aiOrientation = channel->mRotationKeys[rotationIndex].mValue;
		float timeStamp = channel->mRotationKeys[rotationIndex].mTime;
		KeyRotation data;
		data.Orientation = {aiOrientation.w, aiOrientation.x, aiOrientation.y, aiOrientation.z};
		data.Timestamp = timeStamp;
		m_Rotations.push_back(data);
    }

    m_NumScalings = channel->mNumScalingKeys;
    for (int keyIndex = 0; keyIndex < m_NumScalings; ++keyIndex)
    {
		aiVector3D aiScale = channel->mScalingKeys[keyIndex].mValue;
		float timeStamp = channel->mScalingKeys[keyIndex].mTime;
		KeyScale data;
		data.Scale = {aiScale.x, aiScale.y, aiScale.z};
		data.Timestamp = timeStamp;
		m_Scales.push_back(data);
    }
}

void Bone::Update(float animationTime) 
{
    glm::mat4 translation = InterpolatePosition(animationTime);
    glm::mat4 rotation = InterpolateRotation(animationTime);
    glm::mat4 scale = InterpolateScaling(animationTime);
    m_LocalTransform = translation * rotation * scale;
}

int Bone::GetPositionIndex(float animationTime) 
{
    for (int index = 0; index < m_NumPositions - 1; ++index)
    {
		if (animationTime < m_Positions[index + 1].Timestamp) return index;
    }
    assert(0);
}

int Bone::GetRotationIndex(float animationTime)
{
    for (int index = 0; index < m_NumRotations - 1; ++index)
    {
        if (animationTime < m_Rotations[index + 1].Timestamp) return index;
    }
    assert(0);
}

int Bone::GetScaleIndex(float animationTime)
{
    for (int index = 0; index < m_NumScalings - 1; ++index)
    {
        if (animationTime < m_Scales[index + 1].Timestamp) return index;
    }
    assert(0);
}

float Bone::GetScaleFactor(float lastTimeStamp, float nextTimeStamp, float animationTime)
{
    float scaleFactor = 0.0f;
    float midWayLength = animationTime - lastTimeStamp;
    float framesDiff = nextTimeStamp - lastTimeStamp;
    scaleFactor = midWayLength / framesDiff;
    return scaleFactor;
}

glm::mat4 Bone::InterpolatePosition(float animationTime)
{
    if (1 == m_NumPositions) return glm::translate(glm::mat4(1.0f), m_Positions[0].Position);

    int p0Index = GetPositionIndex(animationTime);
    int p1Index = p0Index + 1;
    float scaleFactor = GetScaleFactor(m_Positions[p0Index].Timestamp, m_Positions[p1Index].Timestamp, animationTime);
    glm::vec3 finalPosition = glm::mix(m_Positions[p0Index].Position, m_Positions[p1Index].Position, scaleFactor);

    return glm::translate(glm::mat4(1.0f), finalPosition);
}

glm::mat4 Bone::InterpolateRotation(float animationTime)
{
    if (1 == m_NumRotations)
    {
        auto rotation = glm::normalize(m_Rotations[0].Orientation);
        return glm::toMat4(rotation);
    }

    int p0Index = GetRotationIndex(animationTime);
    int p1Index = p0Index + 1;
    float scaleFactor = GetScaleFactor(m_Rotations[p0Index].Timestamp, m_Rotations[p1Index].Timestamp, animationTime);
    glm::quat finalRotation =
        glm::slerp(m_Rotations[p0Index].Orientation, m_Rotations[p1Index].Orientation, scaleFactor);
    finalRotation = glm::normalize(finalRotation);
    return glm::toMat4(finalRotation);
}

glm::mat4 Bone::InterpolateScaling(float animationTime)
{
    if (1 == m_NumScalings) return glm::scale(glm::mat4(1.0f), m_Scales[0].Scale);

    int p0Index = GetScaleIndex(animationTime);
    int p1Index = p0Index + 1;
    float scaleFactor = GetScaleFactor(m_Scales[p0Index].Timestamp, m_Scales[p1Index].Timestamp, animationTime);
    glm::vec3 finalScale = glm::mix(m_Scales[p0Index].Scale, m_Scales[p1Index].Scale, scaleFactor);
    return glm::scale(glm::mat4(1.0f), finalScale);
}
} // namespace Engine
