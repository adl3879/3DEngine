#include "Scene.h"

#include "Entity.h"
#include "AllComponents.h"
#include "Light.h"
#include "RenderSystem.h"
#include "PhysicsSystem.h"
#include "PhysicsManager.h"

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

    return dst;
}

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

Entity Scene::DuplicateEntity(Entity entity)
{
    const auto &tag = entity.GetComponent<TagComponent>().Tag;
    Entity newEntity = CreateEntity(tag);
    CopyComponentIfExists(AllComponentsExceptIDAndTag{}, newEntity, entity);
    return newEntity;
}

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
