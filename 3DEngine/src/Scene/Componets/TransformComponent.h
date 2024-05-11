#pragma once

#include <glm/glm.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

namespace Engine
{
struct TransformComponent
{
    glm::vec3 Translation = glm::vec3(0, 0, 0);
    glm::quat Rotation = glm::quat(1, 0, 0, 0);
    glm::vec3 Scale = glm::vec3(1, 1, 1);

    glm::vec3 GlobalTranslation = glm::vec3(0, 0, 0);
    glm::quat GlobalRotation = glm::quat(1, 0, 0, 0);
    glm::vec3 GlobalScale = glm::vec3(1, 1, 1);

    glm::mat4 LocalTransform = glm::mat4(1.0f);
    glm::mat4 GlobalTransform = glm::mat4(1.0f);

    bool Dirty = true;
    bool GlobalDirty = true;

    glm::mat4 GetGlobalTransform() const { return GlobalTransform; }
    void SetGlobalTransform(const glm::mat4 &transform) { GlobalTransform = transform; }

    glm::mat4 GetLocalTransform() const { return LocalTransform; }
    void SetLocalTransform(const glm::mat4 &transform) { LocalTransform = transform; }

    void SetLocalRotation(const glm::quat &quat)
    {
        Rotation = quat;
        Dirty = true;
    }
    glm::quat GetLocalRotation() const { return Rotation; }

    void SetGlobalRotation(const glm::quat &quat)
    {
        GlobalRotation = quat;
        Dirty = true;
    }
    glm::quat GetGlobalRotation() const { return GlobalRotation; }

    glm::vec3 GetLocalPosition() const { return Translation; }
    void SetLocalPosition(const glm::vec3 &position)
    {
        Translation = position;
        Dirty = true;
    }

    glm::vec3 GetGlobalPosition() const { return GlobalTranslation; }
    void SetGlobalPosition(const glm::vec3 &position) { GlobalTranslation = position; }

    void SetLocalScale(const glm::vec3 &scale)
    {
        Scale = scale;
        Dirty = true;
    }
    glm::vec3 GetLocalScale() const { return Scale; }

    void SetGlobalScale(const glm::vec3 &scale) { GlobalScale = scale; }
    glm::vec3 GetGlobalScale() const { return GlobalScale; }
};

struct PrevTransformComponent
{
    glm::vec3 Translation = glm::vec3(0, 0, 0);
    glm::quat Rotation = glm::quat(1, 0, 0, 0);
    glm::vec3 Scale = glm::vec3(1, 1, 1);
};
} // namespace Engine
