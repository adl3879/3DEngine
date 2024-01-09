#pragma once

#include <vector>
#include <string>
#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

struct aiNodeAnim;

namespace Engine
{
struct KeyPosition
{
    glm::vec3 Position;
    float Timestamp;
};

struct KeyRotation
{
    glm::quat Orientation;
    float Timestamp;
};

struct KeyScale
{
    glm::vec3 Scale;
    float Timestamp;
};

class Bone
{
  public:
    Bone(const std::string &name, int id, const aiNodeAnim *channel);
    void Update(float animationTime);
    int GetPositionIndex(float animationTime);
    int GetRotationIndex(float animationTime);
    int GetScaleIndex(float animationTime);

    glm::mat4 GetLocalTransform() const { return m_LocalTransform; }
    std::string GetBoneName() const { return m_Name; }
    int GetBoneID() const { return m_ID; }

  private:
    float GetScaleFactor(float lastTimeStamp, float nextTimeStamp, float animationTime);
    glm::mat4 InterpolatePosition(float animationTime);
    glm::mat4 InterpolateRotation(float animationTime);
    glm::mat4 InterpolateScaling(float animationTime);

  private:
    std::vector<KeyPosition> m_Positions;
    std::vector<KeyRotation> m_Rotations;
    std::vector<KeyScale> m_Scales;
    int m_NumPositions, m_NumRotations, m_NumScalings;

	glm::mat4 m_LocalTransform;
    std::string m_Name;
	int m_ID;
};
}
