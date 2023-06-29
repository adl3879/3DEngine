#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#include "Engine.h"
#include <iostream>

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
    std::string Path = "/home/adeleye/Source/3DEngine/src/Sandbox/res/models/box/scene.gltf";
    Engine::Model Model{};

    ModelComponent() { Model = Engine::Model(Path.c_str()); }
    ModelComponent(const ModelComponent &) = default;
    ModelComponent(const std::string &modelPath) : Path(modelPath) { Model = Engine::Model(modelPath.c_str()); }
};

struct CameraComponent
{
    Engine::Camera Camera;
    bool Primary = false;

    CameraComponent() = default;
    CameraComponent(const CameraComponent &) = default;
    CameraComponent(const Engine::Camera &camera) : Camera(camera) {}
};

struct NativeScriptComponent
{
    ScriptableEntity *Instance = nullptr;

    ScriptableEntity *(*InstantiateScript)();
    void (*DestroyScript)(NativeScriptComponent *);

    template <typename T> void Bind()
    {
        InstantiateScript = []() { return static_cast<ScriptableEntity *>(new T()); };
        DestroyScript = [](NativeScriptComponent *nsc)
        {
            delete nsc->Instance;
            nsc->Instance = nullptr;
        };
    }
};

struct LuaScriptComponent
{
    std::vector<std::string> Paths, Names;
    LuaScriptableEntity *Instance = nullptr;

    std::function<LuaScriptableEntity *(void)> InstantiateScript;
    void (*DestroyScript)(LuaScriptComponent *);

    void Bind(const std::string &filepath, const std::string &name)
    {
        Paths.push_back(filepath);
        Names.push_back(name);
        InstantiateScript = [filepath, name]() { return new LuaScriptableEntity(filepath, name); };
        DestroyScript = [](LuaScriptComponent *lsc)
        {
            delete lsc->Instance;
            lsc->Instance = nullptr;
        };
    }
};

struct DirectionalLightComponent
{
    glm::vec3 Direction = {0.0f, 0.0f, 0.0f};
    glm::vec3 Color = {1.0f, 1.0f, 1.0f};
    float Intensity = 1.0f;

    DirectionalLightComponent() = default;
    DirectionalLightComponent(const DirectionalLightComponent &) = default;
    DirectionalLightComponent(const glm::vec3 &direction, const glm::vec3 &color, float intensity)
        : Direction(direction), Color(color), Intensity(intensity)
    {
    }
};

struct PointLightComponent
{
    glm::vec3 Position = {0.0f, 0.0f, 0.0f};
    glm::vec3 Color = {1.0f, 1.0f, 1.0f};
    float Intensity = 1.0f;

    PointLightComponent() = default;
    PointLightComponent(const PointLightComponent &) = default;
    PointLightComponent(const glm::vec3 &position, const glm::vec3 &color, float intensity)
        : Position(position), Color(color), Intensity(intensity)
    {
    }
};

struct SpotLightComponent
{
    glm::vec3 Position = {0.0f, 0.0f, 0.0f};
    glm::vec3 Direction = {0.0f, 0.0f, 0.0f};
    glm::vec3 Color = {1.0f, 1.0f, 1.0f};
    float Intensity = 1.0f;
    float Cutoff = 0.0f;

    SpotLightComponent() = default;
    SpotLightComponent(const SpotLightComponent &) = default;
    SpotLightComponent(const glm::vec3 &position, const glm::vec3 &direction, const glm::vec3 &color, float intensity,
                       float cutoff)
        : Position(position), Direction(direction), Color(color), Intensity(intensity), Cutoff(cutoff)
    {
    }
};
} // namespace Engine