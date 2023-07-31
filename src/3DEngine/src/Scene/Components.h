#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#include "Engine.h"
#include "Light.h"
#include "Model.h"
#include "UUID.h"

#include <iostream>
#include <memory>

namespace Engine
{
struct IDComponent
{
    UUID ID = 0;

    IDComponent() = default;
    IDComponent(const IDComponent &) = default;
    IDComponent(const UUID &uuid) : ID(uuid) {}
};

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
    std::string Path = "Null";
    std::string Name = std::string();
    int EntityID;
    ModelPtr Model = nullptr;

    ModelComponent() = default;
    ModelComponent(const ModelComponent &) = default;

    void Create()
    {
        Model = std::make_shared<Engine::Model>(EntityID, Path, Name);
        LOG_CORE_INFO("Model {0} at {1} Loaded!", Name, Path);
    }
};

struct CameraComponent
{
    Engine::PerspectiveCamera Camera;
    bool Primary = false;

    CameraComponent() = default;
    CameraComponent(const CameraComponent &) = default;
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

class LuaScriptableEntity;

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
    DirectionalLight Light{};

    DirectionalLightComponent() { Light::SetDirectionalLight(&Light); }
    DirectionalLightComponent(const DirectionalLightComponent &) = default;
};

struct PointLightComponent
{
    int Index = 0;
    PointLight Light;

    PointLightComponent() = default;
    PointLightComponent(const PointLightComponent &) = default;
};

struct SpotLightComponent
{
    int Index = 0;
    SpotLight Light;

    SpotLightComponent() = default;
    SpotLightComponent(const SpotLightComponent &) = default;
};

struct VisibilityComponent
{
    bool IsVisible = true;

    VisibilityComponent() = default;
    VisibilityComponent(const VisibilityComponent &) = default;
};
} // namespace Engine