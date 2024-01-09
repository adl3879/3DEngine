#include "Scene.h"

#include "Entity.h"
#include "AllComponents.h"
#include "Light.h"
#include "PhysicsSystem.h"
#include "TransformSystem.h"
#include "ScriptingSystem.h"
#include "SceneRenderer.h"
#include "PhysicsManager.h"
#include "Components.h"
#include "ScriptingEngineNet.h"

#include <stack>

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
    for (const auto entity : idView)
    {
        UUID uuid = srcSceneRegistry.get<IDComponent>(entity).ID;
        const auto &name = srcSceneRegistry.get<TagComponent>(entity).Tag;
        Entity newEntity = dst->CreateEntityWithUUID(uuid, name);
        enttMap[uuid] = static_cast<entt::entity>(newEntity);
    }

    // copy components
    CopyComponent(AllComponentsExceptIDAndTag{}, dstSceneRegistry, srcSceneRegistry, enttMap);

    dst->SetEnvironment(src->GetEnvironment());

    return dst;
}

Scene::Scene() { New("Untitled Scene"); }

Scene::Scene(const std::string &name) { New(name); }

void Scene::New(const std::string &name)
{
    m_SceneName = name;

    m_Systems = std::vector<SystemRef>();
    m_EditorCamera = std::make_shared<EditorCamera>(-45.0f, 1.778f, 0.1f, 100.0f);
    m_Environment = std::make_shared<Environment>();
    m_Lights = std::make_shared<Light>();

    PhysicsManager::Get().Init(this);

    // Add systems
    m_Systems.push_back(std::make_shared<PhysicsSystem>(this));
    m_Systems.push_back(std::make_shared<TransformSystem>(this));
    m_Systems.push_back(std::make_shared<ScriptingSystem>(this));

    m_SceneRenderer = new SceneRenderer();
    m_SceneRenderer->Init();
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

void Scene::OnRuntimeStart()
{
    PhysicsManager::Get().Start();
    for (const auto &system : m_Systems) system->OnRuntimeStart();
}

void Scene::OnRuntimeStop()
{
    PhysicsManager::Get().Stop();
    for (const auto &system : m_Systems) system->OnRuntimeStop();
}

void Scene::StepRuntimeFrame(int step) { PhysicsManager::Get().StepFrame(step); }

void Scene::OnUpdate(float dt)
{
    for (const auto &system : m_Systems) system->Update(dt);

    auto view = m_Registry.view<DirectionalLightComponent>();
    for (const auto entity : view)
    {
        auto &light = view.get<DirectionalLightComponent>(entity);
        m_Lights->SetDirectionalLight(light.Enabled ? &light.Light : nullptr);
    }

    auto pointLightView = m_Registry.view<PointLightComponent>();
    for (const auto entity : pointLightView)
    {
        auto &light = pointLightView.get<PointLightComponent>(entity);
        m_Lights->SetPointLight(light.Enabled ? &light.Light : nullptr, light.Index);
    }

    auto spotLightView = m_Registry.view<SpotLightComponent>();
    for (const auto entity : spotLightView)
    {
        auto &light = spotLightView.get<SpotLightComponent>(entity);
        auto spotLight = light.Enabled ? light.Light : SpotLight();
        m_Lights->SetSpotLight(light.Enabled ? &light.Light : nullptr, light.Index);
    }
}

void Scene::OnFixedUpdate(float dt)
{
    if (!m_IsPaused)
    {
        for (const auto &system : m_Systems) system->FixedUpdate(dt);
    }
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
    //! Check if there is a memory issue
    auto view = m_Registry.view<TagComponent>();
    for (auto entity : view)
    {
        auto &tag = view.get<TagComponent>(entity);
        auto entt = new Entity(entity, this);
        if (tag.Tag == name) return entt;
    }
    return nullptr;
}

Entity Scene::GetEntityByUUID(UUID uuid)
{
    // O(1)
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
    if (!parent.Children.empty())
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
    Entity newEntity = {};
    std::string tag = entity.GetComponent<TagComponent>().Tag;

    newEntity = CreateEntity(tag);
    CopyComponentIfExists(AllComponentsExceptIDAndTag{}, newEntity, entity);

    return newEntity;
}

Entity Scene::DuplicateEntityRecursiveW(Entity originalEntity, std::unordered_map<UUID, Entity> &entityMap)
{
    if (!originalEntity)
    {
        // Return an invalid entity for invalid input
        return {};
    }

    // Check if the original entity has already been duplicated
    if (entityMap.find(originalEntity.GetUUID()) != entityMap.end())
    {
        // Return the duplicated entity from the map
        return entityMap[originalEntity.GetUUID()];
    }

    // Duplicate the original entity and generate a new UUID
    Entity newEntity = DuplicateEntity(originalEntity);

    // Process child entities
    auto &parent = originalEntity.GetComponent<ParentComponent>();
    for (const auto &e : parent.ChildEntities)
    {
        // Recursively duplicate child entities and link to the new parent entity
        Entity newChildEntity = DuplicateEntityRecursiveW(e, entityMap);
    }

    // Return the duplicated entity
    return newEntity;
}

Entity Scene::DuplicateEntityRecursive(Entity entity)
{
   /* std::unordered_map<UUID, Entity> entityMap;
    return DuplicateEntityRecursiveW(entity, entityMap);*/

	Entity newEntity = DuplicateEntity(entity);

	auto &parent = entity.GetComponent<ParentComponent>();
    if (!parent.Children.empty())
    {
        for (const auto &child : parent.Children)
        {
            DuplicateEntityRecursive(GetEntityByUUID(child));
        }
    }
    return newEntity;
}
    
void Scene::Merge(std::shared_ptr<Scene> src)
{
    auto &srcSceneRegistry = src->m_Registry;
    std::unordered_map<UUID, entt::entity> enttMap;

    auto idView = srcSceneRegistry.view<IDComponent>();
    for (auto entity : idView)
    {
        UUID uuid = srcSceneRegistry.get<IDComponent>(entity).ID;
        const auto &name = srcSceneRegistry.get<TagComponent>(entity).Tag;
        Entity newEntity = CreateEntityWithUUID(uuid, name);
        enttMap[uuid] = (entt::entity)newEntity;
    }

    // copy components
    CopyComponent(AllComponentsExceptIDAndTag{}, m_Registry, srcSceneRegistry, enttMap);
}

void Scene::OnRuntimeUpdate(float dt)
{
    if (!m_IsPaused)
    {
        for (const auto &system : m_Systems) system->OnRuntimeUpdate(dt);

        glm::mat4 cameraTransform;
        {
            auto view = m_Registry.view<CameraComponent>();
            for (auto entity : view)
            {
                auto cameraComponent = view.get<CameraComponent>(entity);
                if (cameraComponent.Primary)
                {
                    m_MainCamera = cameraComponent.Camera;
                    m_MainCamera->OnWindowResize(m_ViewportSize.x, m_ViewportSize.y);
                }
            }
        }
    }

    if (m_MainCamera != nullptr)
    {
        m_SceneRenderer->BeginRenderScene(m_MainCamera->GetProjectionMatrix(), m_MainCamera->GetViewMatrix(),
                                          m_MainCamera->GetPosition());
        m_SceneRenderer->RenderScene(*this, *m_Framebuffer);
    }
}

void Scene::OnUpdateEditor(float dt, EditorCamera &camera)
{
    const auto projection = camera.GetProjectionMatrix();
    const auto view = camera.GetViewMatrix();

    m_SceneRenderer->BeginRenderScene(projection, view, camera.GetPosition());
    m_SceneRenderer->RenderScene(*this, *m_Framebuffer);
}
void Scene::GenerateNETSolution() { ScriptingEngineNet::Get().GenerateSolution(); }

void Scene::InitTextEditor(const std::filesystem::path &filepath)
{
    if (m_TextEditors[filepath] == nullptr)
    {
        m_TextEditors[filepath] = std::make_shared<ImGuiTextEditor>();
    }
    m_TextEditors[filepath]->Init(filepath);
}

void Scene::RenderTextEditors()
{
    for (const auto &textEditor : m_TextEditors) textEditor.second->Render();
}

bool Scene::IsTextEditorFocused()
{
    return std::any_of(m_TextEditors.begin(), m_TextEditors.end(),
                       [](const std::pair<std::filesystem::path, ImGuiTextEditorRef> &textEditor)
                       { return textEditor.second->IsWindowFocused(); });
}
} // namespace Engine
