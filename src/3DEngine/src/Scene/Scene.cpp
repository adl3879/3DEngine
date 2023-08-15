#include "Scene.h"

#include "Entity.h"
#include "Components.h"
#include "Light.h"
#include "RenderSystem.h"
#include "PhysicsSystem.h"
#include "PhysicsManager.h"

namespace Engine
{
Scene::Scene()
{
    m_Systems = std::vector<SystemRef>();

    PhysicsManager::Get().Init(this);

    // Add systems
    m_Systems.push_back(std::make_shared<PhysicsSystem>(this));
}

Scene::~Scene() {}

void Scene::OnAttach()
{
    for (const auto &system : m_Systems) system->Init();
}

void Scene::OnDetach()
{
    for (const auto &system : m_Systems) system->Exit();
}

void Scene::OnUpdate(float dt)
{
    for (const auto &system : m_Systems) system->Update(dt);
}

void Scene::OnFixedUpdate(float dt)
{
    for (const auto &system : m_Systems) system->FixedUpdate(dt);
}

Entity Scene::CreateEntity(const std::string &name) { return CreateEntityWithUUID(UUID(), name); }

Entity Scene::CreateEntityWithUUID(UUID uuid, const std::string &name)
{
    Entity entity = {m_Registry.create(), this};
    auto tag = entity.AddComponent<TagComponent>();

    entity.GetComponent<TagComponent>().Tag = name.empty() ? "Unnamed Entity" : name;
    entity.AddComponent<IDComponent>(uuid);
    entity.AddComponent<TransformComponent>();
    entity.AddComponent<VisibilityComponent>();

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

void Scene::DestroyEntity(Entity entity) { m_Registry.destroy(entity); }

void Scene::OnUpdateRuntime(float dt)
{
    // update scripts
    {
        m_Registry.view<LuaScriptComponent>().each(
            [=](auto entity, auto &lsc)
            {
                if (!lsc.Instance)
                {
                    lsc.Instance = lsc.InstantiateScript();
                    lsc.Instance->m_Entity = Entity{entity, this};
                    lsc.Instance->Setup();
                    lsc.Instance->m_Script->GetCallbackFunctions().OnCreate();
                }
                lsc.Instance->m_Script->ReloadScriptIfModified();
                lsc.Instance->m_Script->GetCallbackFunctions().OnUpdate(dt);
            });
    }

    glm::mat4 cameraTransform;
    {
        auto view = m_Registry.view<TransformComponent, CameraComponent>();
        for (auto entity : view)
        {
            auto [cameraComponent, transformComponent] = view.get<CameraComponent, TransformComponent>(entity);
            if (cameraComponent.Primary)
            {
                cameraComponent.Camera.SetPosition(transformComponent.Translation);
                cameraComponent.Camera.SetRotation(transformComponent.Rotation);

                m_MainCamera = std::make_shared<PerspectiveCamera>(cameraComponent.Camera);
            }
            cameraTransform = view.get<TransformComponent>(entity).GetTransform();
        }
    }
}

void Scene::OnUpdateEditor(float dt, EditorCamera &camera) {}
} // namespace Engine
