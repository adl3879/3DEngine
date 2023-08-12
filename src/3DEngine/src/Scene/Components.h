#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#include "Engine.h"
#include "Light.h"
#include "Model.h"
#include "UUID.h"
#include "Asset.h"
#include "SkyLight.h"
#include "JoltPhysicsIncludes.h"

#include <iostream>
#include <memory>

namespace Engine
{
using namespace JPH;

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

struct MeshComponent
{
    AssetHandle Handle = 0;
    AssetHandle MaterialHandle = 0;

    MeshComponent() = default;
    MeshComponent(const MeshComponent &) = default;
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

struct SkyLightComponent
{
    AssetHandle TextureHandle = 0;
    SkyLight *Light = nullptr;

    SkyLightComponent() = default;
    SkyLightComponent(const SkyLightComponent &) = default;

    ~SkyLightComponent() { Remove(); }

    void Use(AssetHandle handle, const std::size_t resolution)
    {
        Remove();
        TextureHandle = handle;
        Light = new SkyLight();
        Light->Init(handle, resolution);
    }

    void Remove()
    {
        // TODO: completely remove and unbind the sky light as if it was never there
        TextureHandle = 0;
        Light->Destroy();
    }
};

struct VisibilityComponent
{
    bool IsVisible = true;

    VisibilityComponent() = default;
    VisibilityComponent(const VisibilityComponent &) = default;
};

struct RigidBodyComponent
{
    EMotionType MotionType = EMotionType::Static;
    uint32_t LayerID = 0;

    float Mass = 1.0f;

    RigidBodyComponent() = default;
    RigidBodyComponent(const RigidBodyComponent &) = default;
};
} // namespace Engine