#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Engine.h"
#include "Light.h"
#include "Model.h"
#include "UUID.h"
#include "Asset.h"
#include "Animator.h"

#include "TransformComponent.h"

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
    bool IsPrefabRoot = false;
    bool IsRoot = false;
	bool IsFirstChild = false;

    TagComponent() = default;
    TagComponent(const TagComponent &) = default;
    explicit TagComponent(const std::string &tag) : Tag(tag) {}
};

struct ParentComponent
{
    UUID Parent;
    bool HasParent = false;
    std::vector<UUID> Children;

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

struct PrefabInstanceComponent 
{
	UUID PrefabID = 0;

	PrefabInstanceComponent() = default;
	PrefabInstanceComponent(const PrefabInstanceComponent &) = default;
};

struct MeshComponent
{
    AssetHandle Handle = 0;
    AssetHandle MaterialHandle;

	MeshComponent() = default;
    MeshComponent(const MeshComponent &) = default;
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
