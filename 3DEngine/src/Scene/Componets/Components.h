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
#include "Prefab.h"
#include "Animator.h"

#include <memory>

namespace Engine
{
struct IDComponent
{
    UUID ID = 0;

    IDComponent() = default;
    IDComponent(const IDComponent &) = default;
    explicit IDComponent(const UUID &uuid) : ID(uuid) {}
};

struct TagComponent
{
    std::string Tag;
    bool IsPrefab = false;

    TagComponent() = default;
    TagComponent(const TagComponent &) = default;
    explicit TagComponent(const std::string &tag) : Tag(tag) {}
};

struct ParentComponent
{
    UUID Parent;
    bool HasParent = false;
    std::vector<UUID> Children;

    // stores entt entity handle
    std::vector<Entity> ChildEntities;

    bool RemoveChild(UUID entityId)
    {
        for (auto it = Children.begin(); it != Children.end(); ++it)
        {
            if (*it == entityId)
            {
                Children.erase(it);
                return true;
            }
        }
        return false;
    }

    ParentComponent() = default;
    ParentComponent(const ParentComponent &) = default;
};

struct PrefabComponent
{
    PrefabRef Instance = nullptr;

    PrefabComponent() = default;
    PrefabComponent(const PrefabComponent &) = default;
    explicit PrefabComponent(const PrefabRef &prefab) : Instance(prefab) {}
};

struct TransformComponent
{
    glm::vec3 Translation = {0.0f, 0.0f, 0.0f};
    glm::vec3 Rotation = {0.0f, 0.0f, 0.0f};
    glm::vec3 Scale = {1.0f, 1.0f, 1.0f};

    glm::vec3 LocalTranslation = {0.0f, 0.0f, 0.0f};
    glm::vec3 LocalRotation = {0.0f, 0.0f, 0.0f};
    glm::vec3 LocalScale = {1.0f, 1.0f, 1.0f};

    TransformComponent() = default;
    TransformComponent(const TransformComponent &) = default;
    explicit TransformComponent(const glm::vec3 &translation) : Translation(translation) {}

    [[nodiscard]] glm::mat4 GetTransform() const
    {
        const glm::mat4 rotation = glm::toMat4(glm::quat(Rotation));
        return glm::translate(glm::mat4(1.0f), Translation) * rotation * glm::scale(glm::mat4(1.0f), Scale);
    }

    void SetTransform(const glm::mat4 &transform)
    {
        Translation = glm::vec3(transform[3]);
        Scale = glm::vec3(glm::length(transform[0]), glm::length(transform[1]), glm::length(transform[2]));

        auto rotation = glm::mat4(transform);
        rotation[3] = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

        Rotation = glm::eulerAngles(glm::quat_cast(rotation));
    }

    [[nodiscard]] glm::mat4 GetLocalTransform() const
    {
        const glm::mat4 rotation = glm::toMat4(glm::quat(LocalRotation));
        return glm::translate(glm::mat4(1.0f), LocalTranslation) * rotation * glm::scale(glm::mat4(1.0f), LocalScale);
    }

    void SetLocalTransform(const glm::mat4 &transform)
    {
        LocalTranslation = glm::vec3(transform[3]);
        LocalScale = glm::vec3(glm::length(transform[0]), glm::length(transform[1]), glm::length(transform[2]));

        auto rotation = glm::mat4(transform);
        rotation[3] = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

        LocalRotation = glm::eulerAngles(glm::quat_cast(rotation));
    }
};

struct MeshComponent
{
    AssetHandle Handle = 0;
    AssetHandle MaterialHandle;
};

struct StaticMeshComponent : public MeshComponent
{
    StaticMeshComponent() = default;
    StaticMeshComponent(const StaticMeshComponent &) = default;
};

struct SkinnedMeshComponent : public MeshComponent
{
	SkinnedMeshComponent() = default;
    SkinnedMeshComponent(const SkinnedMeshComponent &) = default;
};

struct CameraComponent
{
    PerspectiveCameraRef Camera;
    bool Primary = false;

    CameraComponent() { Camera = std::make_shared<PerspectiveCamera>(); }
    CameraComponent(const CameraComponent &) = default;
};

struct DirectionalLightComponent
{
    DirectionalLight Light{};
    bool Enabled = true;

    DirectionalLightComponent() = default;
    DirectionalLightComponent(const DirectionalLightComponent &) = default;
};

struct PointLightComponent
{
    int Index = 0;
    bool Enabled = true;
    PointLight Light;

    PointLightComponent() = default;
    PointLightComponent(const PointLightComponent &) = default;
};

struct SpotLightComponent
{
    int Index = 0;
    bool Enabled = true;
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

struct NetScriptComponent
{
    AssetHandle Handle = 0;

    NetScriptComponent() = default;
    NetScriptComponent(const NetScriptComponent &) = default;
};

struct AnimationControllerComponent
{
	Animator *Animator = nullptr;
	std::vector<Animation *> Animations;
	std::string CurrentAnimation;

	void AddAnimation(Animation* animation)
	{
		static std::string defaultAnimation = animation->GetName();
		Animations.push_back(animation);
		CurrentAnimation = defaultAnimation;
	}

	AnimationControllerComponent() = default;
	AnimationControllerComponent(const AnimationControllerComponent &) = default;
};
} // namespace Engine
