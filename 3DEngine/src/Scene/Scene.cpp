#include "Scene.h"

#include "Entity.h"
#include "AllComponents.h"
#include "Light.h"
#include "PhysicsSystem.h"
#include "TransformSystem.h"
#include "SceneRenderer.h"
#include "PhysicsManager.h"
#include "Components.h"
#include "RenderCommand.h"

namespace Engine
{
template <typename... Component>
static void CopyComponent(entt::registry &dst, entt::registry &src,
                          const std::unordered_map<UUID, entt::entity> &enttMap)
{
    // clang-format off
    ([&]()
    {
        auto view = src.view<Component>();
        for (auto srcEntity : view)
        {
            entt::entity dstEntity = enttMap.at(src.get<IDComponent>(srcEntity).ID);

            auto &srcComponent = src.get<Component>(srcEntity);
            dst.emplace_or_replace<Component>(dstEntity, srcComponent);
        }
    }(), ...);
    // clang-format on
}

template <typename... Component>
static void CopyComponent(ComponentExceptIDAndTagGroup<Component...>, entt::registry &dst, entt::registry &src,
                          const std::unordered_map<UUID, entt::entity> &enttMap)
{
    CopyComponent<Component...>(dst, src, enttMap);
}

template <typename... Component> static void CopyComponentIfExists(Entity dst, Entity src)
{
    // clang-format off
    ([&]()
    {
        if (src.HasComponent<Component>()) dst.AddOrReplaceComponent<Component>(src.GetComponent<Component>());
    }(), ...);
    // clang-format on
}

template <typename... Component>
static void CopyComponentIfExists(ComponentExceptIDAndTagGroup<Component...>, Entity dst, Entity src)
{
    CopyComponentIfExists<Component...>(dst, src);
}

std::shared_ptr<Scene> Scene::Copy(std::shared_ptr<Scene> src)
{
    std::shared_ptr<Scene> dst = std::make_shared<Scene>();

    auto &srcSceneRegistry = src->m_Registry;
    auto &dstSceneRegistry = dst->m_Registry;

    std::unordered_map<UUID, entt::entity> enttMap;

    auto idView = srcSceneRegistry.view<IDComponent>();
    for (auto entity : idView)
    {
        UUID uuid = srcSceneRegistry.get<IDComponent>(entity).ID;
        const auto &name = srcSceneRegistry.get<TagComponent>(entity).Tag;
        Entity newEntity = dst->CreateEntityWithUUID(uuid, name);
        enttMap[uuid] = (entt::entity)newEntity;
    }

    // copy components
    CopyComponent(AllComponentsExceptIDAndTag{}, dstSceneRegistry, srcSceneRegistry, enttMap);

	dst->SetEnvironment(src->GetEnvironment());

    return dst;
}

Scene::Scene()
{
	m_SceneName = "Untitled Scene";

    m_Systems = std::vector<SystemRef>();
    m_EditorCamera = std::make_shared<EditorCamera>(-45.0f, 1.778f, 0.1f, 100.0f);
    m_Environment = std::make_shared<Environment>();
    m_Lights = std::make_shared<Light>();

    PhysicsManager::Get().Init(this);

    // Add systems
    m_Systems.push_back(std::make_shared<PhysicsSystem>(this));
    m_Systems.push_back(std::make_shared<TransformSystem>(this));

    m_SceneRenderer = new SceneRenderer();
    m_SceneRenderer->Init();
}

Scene::~Scene() {}

void Scene::OnAttach()
{
    ScriptEngine::Init();
    for (const auto &system : m_Systems) system->Init();
}

void Scene::OnDetach()
{
    for (const auto &system : m_Systems) system->Exit();
}

void Scene::OnRuntimeStart()
{
	PhysicsManager::Get().Start();
    // Scripting
    {
        ScriptEngine::OnRuntimeStart(this);
        // Instantiate all script entities

        auto view = m_Registry.view<ScriptComponent>();
        for (auto e : view)
        {
            Entity entity = {e, this};
            ScriptEngine::OnCreateEntity(entity);
        }
    }
}

void Scene::OnRuntimeStop() 
{
	PhysicsManager::Get().Stop();
	// Scripting
	{
		ScriptEngine::OnRuntimeStop();
	}
}

void Scene::StepRuntimeFrame(int step) 
{
	PhysicsManager::Get().StepFrame(step);
}

void Scene::OnUpdate(float dt)
{
    for (const auto &system : m_Systems) system->Update(dt);

    auto view = m_Registry.view<DirectionalLightComponent>();
    for (auto entity : view)
    {
        auto &light = view.get<DirectionalLightComponent>(entity);
        m_Lights->SetDirectionalLight(light.Enabled ? &light.Light : nullptr);
    }

    auto pointLightView = m_Registry.view<PointLightComponent>();
    for (auto entity : pointLightView)
    {
        auto &light = pointLightView.get<PointLightComponent>(entity);
        m_Lights->SetPointLight(light.Enabled ? &light.Light : nullptr, light.Index);
    }

    auto spotLightView = m_Registry.view<SpotLightComponent>();
    for (auto entity : spotLightView)
    {
        auto &light = spotLightView.get<SpotLightComponent>(entity);
        auto spotLight = light.Enabled ? light.Light : SpotLight();
        m_Lights->SetSpotLight(light.Enabled ? &light.Light : nullptr, light.Index);
    }
}

void Scene::OnFixedUpdate(float dt)
{
    for (const auto &system : m_Systems) system->FixedUpdate(dt);
}

Entity Scene::CreateEntity(const std::string &name) { return CreateEntityWithUUID(UUID(), name); }

Entity Scene::CreateEntityWithUUID(UUID uuid, const std::string &name)
{
    Entity entity = {m_Registry.create(), this};

    entity.AddComponent<IDComponent>(uuid);
	entity.AddComponent<TagComponent>();
	entity.AddComponent<ParentComponent>();
    entity.AddComponent<TransformComponent>();
    entity.AddComponent<VisibilityComponent>();

	auto &tagComponent = entity.GetComponent<TagComponent>();
    tagComponent.Tag = name.empty() ? "Unnamed Entity" : name;

	m_EntityMap[uuid] = entity;

    return entity;
}

Entity *Scene::GetEntity(const std::string &name)
{
    auto view = m_Registry.view<TagComponent>();
    for (auto entity : view)
    {
        auto &tag = view.get<TagComponent>(entity);
        auto entt = Entity{entity, this};
        if (tag.Tag == name) return &entt;
    }
    return nullptr;
}

Entity Scene::GetEntityByUUID(UUID uuid)
{
	// O(n)
    if (m_EntityMap.find(uuid) != m_EntityMap.end()) return {m_EntityMap.at(uuid), this};

    return {};
}

Entity Scene::FindEntityByName(std::string_view name) 
{
	auto view = m_Registry.view<TagComponent>();
	for (auto entity : view)
	{
		const auto &tag = view.get<TagComponent>(entity);
		if (tag.Tag == name) return {entity, this};
	}
	return {};	
}

void Scene::DestroyEntity(Entity entity) 
{
	m_EntityMap.erase(entity.GetComponent<IDComponent>().ID);
	m_Registry.destroy(entity);
}

void Scene::DestroyEntityRecursive(Entity entity) 
{
	auto &parent = entity.GetComponent<ParentComponent>();
	if (parent.Children.size() > 0)
	{
		for (const auto &child : parent.Children)
		{
			DestroyEntityRecursive(GetEntityByUUID(child));
		}
	}
	DestroyEntity(entity);
}

Entity Scene::DuplicateEntity(Entity entity)
{
	ParentComponent parent = entity.GetComponent<ParentComponent>();
	Entity newEntity = {};
    if (parent.Children.size() <= 0)
    {
        std::string tag = entity.GetComponent<TagComponent>().Tag;
		newEntity = CreateEntity(tag);
        CopyComponentIfExists(AllComponentsExceptIDAndTag{}, newEntity, entity);
		if (parent.HasParent)
		{
			auto &parentParentComp = GetEntityByUUID(parent.Parent).GetComponent<ParentComponent>();
			parentParentComp.Children.push_back(newEntity.GetComponent<IDComponent>().ID);
		}
    }
    return newEntity;
}

void Scene::OnUpdateRuntime(float dt)
{
    if (!m_IsPaused)
	{
		// Update scripts
		{
			// C# Entity OnUpdate
			auto view = m_Registry.view<ScriptComponent>();
			for (auto e : view)
			{
				Entity entity = {e, this};
				ScriptEngine::OnUpdateEntity(entity, dt);
			}
		}

		glm::mat4 cameraTransform;
		{
			auto view = m_Registry.view<TransformComponent, CameraComponent>();
			for (auto entity : view)
			{
				auto [cameraComponent, transformComponent] = view.get<CameraComponent, TransformComponent>(entity);
				if (cameraComponent.Primary)
				{
					// TODO: fix runtime camera
					cameraComponent.Camera->SetPosition(transformComponent.Translation);
					cameraComponent.Camera->SetRotation(transformComponent.Rotation);
					m_MainCamera = cameraComponent.Camera;
					m_MainCamera->OnWindowResize(m_ViewportSize.x, m_ViewportSize.y);
				}
			}
		}
	}
    RenderCommand::SetClearColor({0, 0, 0});
    RenderCommand::Clear();

    if (m_MainCamera != nullptr)
    {
        m_SceneRenderer->BeginRenderScene(m_MainCamera->GetProjectionMatrix(), m_MainCamera->GetViewMatrix(),
                                          m_MainCamera->GetPosition());
        m_SceneRenderer->RenderScene(*this, *m_Framebuffer);
    }
}

void Scene::OnUpdateEditor(float dt, EditorCamera &camera)
{
    m_SceneRenderer->BeginRenderScene(camera.GetProjectionMatrix(), camera.GetViewMatrix(), camera.GetPosition());
    m_SceneRenderer->RenderScene(*this, *m_Framebuffer);
}

} // namespace Engine
