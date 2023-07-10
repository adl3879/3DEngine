#include "Scene.h"

#include "Entity.h"
#include "Components.h"
#include "Light.h"
#include "RenderSystem.h"

namespace Engine
{
Scene::Scene() {}

Scene::~Scene() {}

Entity Scene::CreateEntity(const std::string &name)
{
    Entity entity = {m_Registry.create(), this};
    auto tag = entity.AddComponent<TagComponent>();

    entity.GetComponent<TagComponent>().Tag = name.empty() ? "Unnamed Entity" : name;
    entity.AddComponent<TransformComponent>();

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
        m_Registry.view<NativeScriptComponent>().each(
            [=](auto entity, auto &nsc)
            {
                if (!nsc.Instance)
                {
                    nsc.Instance = nsc.InstantiateScript();
                    nsc.Instance->m_Entity = Entity{entity, this};
                    nsc.Instance->OnCreate();
                }
                nsc.Instance->OnUpdate(dt);
            });

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
    {
        // Renderer3D::BeginScene(*m_MainCamera);
        // {
        //     auto view = m_Registry.view<TransformComponent, ModelComponent>();
        //     for (auto entity : view)
        //     {
        //         auto [transform, model] = view.get<TransformComponent, ModelComponent>(entity);
        //         Renderer3D::DrawModel(model.Model, transform.GetTransform());
        //     }
        // }
        // Renderer3D::EndScene();
    }
}

void Scene::OnUpdateEditor(float dt, EditorCamera &camera)
{
    // Renderer3D::BeginScene(camera);
    // {
    //     auto view = m_Registry.view<TransformComponent, ModelComponent>();
    //     for (auto entity : view)
    //     {
    //         auto [transform, model] = view.get<TransformComponent, ModelComponent>(entity);
    //         Renderer3D::DrawModel(model.Model, transform.GetTransform());
    //         Renderer3D::EndScene();
    //     }
    // }
}
} // namespace Engine
