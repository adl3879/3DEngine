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

struct ParentComponent
{
    UUID Parent;
	bool HasParent = false;
	std::vector<UUID> Children = std::vector<UUID>();

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
	}

	ParentComponent() = default;
	ParentComponent(const ParentComponent &) = default;
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
    TransformComponent(const glm::vec3 &translation) : Translation(translation) {}

    glm::mat4 GetTransform() const
    {
        glm::mat4 rotation = glm::toMat4(glm::quat(Rotation));
        return glm::translate(glm::mat4(1.0f), Translation) * rotation * glm::scale(glm::mat4(1.0f), Scale);
    }

    void SetTransform(const glm::mat4 &transform)
    {
        Translation = glm::vec3(transform[3]);
        Scale = glm::vec3(glm::length(transform[0]), glm::length(transform[1]), glm::length(transform[2]));

        glm::mat4 rotation = glm::mat4(transform);
        rotation[3] = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

        Rotation = glm::eulerAngles(glm::quat_cast(rotation));
    }

	glm::mat4 GetLocalTransform() const
	{
        glm::mat4 rotation = glm::toMat4(glm::quat(LocalRotation));
        return glm::translate(glm::mat4(1.0f), LocalTranslation) * rotation * glm::scale(glm::mat4(1.0f), LocalScale);
	}

	void SetLocalTransform(const glm::mat4 &transform)
	{
        LocalTranslation = glm::vec3(transform[3]);
        LocalScale = glm::vec3(glm::length(transform[0]), glm::length(transform[1]), glm::length(transform[2]));

        glm::mat4 rotation = glm::mat4(transform);
        rotation[3] = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

        LocalRotation = glm::eulerAngles(glm::quat_cast(rotation));
    }
};

struct MeshComponent
{
    AssetHandle Handle = 0;
    AssetHandle MaterialHandle = 0;

    // only for built-in meshes
    ModelRef ModelResource = nullptr;

    MeshComponent() = default;
    MeshComponent(const MeshComponent &) = default;
};

struct CameraComponent
{
    PerspectiveCameraRef Camera;
    bool Primary = false;

	CameraComponent() 
	{
		Camera = std::make_shared<PerspectiveCamera>();
	}
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

struct ScriptComponent
{
    std::string ClassName = "None";

	ScriptComponent() = default;
	ScriptComponent(const ScriptComponent &) = default;
};
} // namespace Engine
