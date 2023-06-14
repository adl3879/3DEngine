#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#include "Camera.h"
#include "Model.h"

namespace Engine
{

struct TagComponent
{
    std::string Tag;

    TagComponent() = default;
    TagComponent(const TagComponent &) = default;
    TagComponent(const std::string &tag) : Tag(tag) {}
};

struct TransformComponent
{
    glm::vec3 Translation = {0.0f, 0.0f, 0.0f};
    glm::vec3 Rotation = {0.0f, 0.0f, 0.0f};
    glm::vec3 Scale = {1.0f, 1.0f, 1.0f};

    TransformComponent() = default;
    TransformComponent(const TransformComponent &) = default;
    TransformComponent(const glm::vec3 &translation) : Translation(translation) {}

    glm::mat4 GetTransform() const
    {
        glm::mat4 rotation = glm::toMat4(glm::quat(Rotation));
        return glm::translate(glm::mat4(1.0f), Translation) * rotation * glm::scale(glm::mat4(1.0f), Scale);
    }
};

struct ModelComponent
{
    std::string Path;
    Engine::Model Model{};

    ModelComponent() = default;
    ModelComponent(const ModelComponent &) = default;
    ModelComponent(const std::string &modelPath) : Path(modelPath) { Model = Engine::Model(modelPath.c_str()); }
};

struct CameraComponent
{
    Engine::Camera Camera;
    bool Primary = true;

    CameraComponent() = default;
    CameraComponent(const CameraComponent &) = default;
    CameraComponent(const Engine::Camera &camera) : Camera(camera) {}
};

struct LightComponent
{
    Engine::Light Light{LightType::Directional};
    LightType Type;

    LightComponent() = default;
    LightComponent(const LightComponent &) = default;
    LightComponent(LightType type) : Type(type) { Light = Engine::Light(type); }
};
} // namespace Engine