#include "Scene.h"

#include "Entity.h"
#include "Components.h"
#include "Camera.h"
#include "Renderer.h"

namespace Engine
{
Scene::Scene()
{
    m_EditorCamera = std::make_shared<Camera>();
    m_EditorCamera->SetPerspective(45.0f, 1.778f, 0.1f, 100.0f);
    m_SceneCamera = m_EditorCamera;
}

Scene::~Scene() {}

Entity Scene::CreateEntity(const std::string &name)
{
    Entity entity = {m_Registry.create(), this};
    auto tag = entity.AddComponent<TagComponent>();

    entity.GetComponent<TagComponent>().Tag = name.empty() ? "Unnamed Entity" : name;
    entity.AddComponent<TransformComponent>();

    return entity;
}

void Scene::DestroyEntity(Entity entity) { m_Registry.destroy(entity); }

void Scene::OnUpdate(float dt)
{
    Light *mainLight = nullptr;
    {
        auto view = m_Registry.view<TransformComponent, LightComponent>();
        for (auto entity : view)
        {
            auto &lightComponent = view.get<LightComponent>(entity);
            mainLight = &lightComponent.Light;
        }
    }

    Camera *mainCamera = nullptr;
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

                mainCamera = &cameraComponent.Camera;
            }
            cameraTransform = view.get<TransformComponent>(entity).GetTransform();
        }
    }
    if (mainCamera)
        m_SceneCamera = std::make_shared<Camera>(*mainCamera);
    else
        m_SceneCamera = m_EditorCamera;

    {
        Renderer3D::BeginScene(*m_SceneCamera, *mainLight);
        {
            auto view = m_Registry.view<TransformComponent, ModelComponent>();
            for (auto entity : view)
            {
                auto [transform, model] = view.get<TransformComponent, ModelComponent>(entity);
                Renderer3D::DrawModel(model.Model, transform.GetTransform());
            }
        }
        Renderer3D::EndScene();
    }
}
} // namespace Engine
