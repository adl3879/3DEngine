#include "Scene.h"

#include "Entity.h"
#include "Components.h"
#include "Camera.h"
#include "Renderer.h"

namespace Engine
{
Scene::~Scene() {}

Entity Scene::CreateEntity(const std::string &name)
{
    Entity entity = {m_Registry.create(), this};
    auto tag = entity.AddComponent<TagComponent>();

    entity.GetComponent<TagComponent>().Tag = name.empty() ? "Unnamed Entity" : name;
    entity.AddComponent<TransformComponent>();

    return entity;
}

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
            auto &cameraComponent = view.get<CameraComponent>(entity);
            mainCamera = &cameraComponent.Camera;
            cameraTransform = view.get<TransformComponent>(entity).GetTransform();
        }
    }
    if (mainCamera)
    {
        Renderer3D::BeginScene(*mainCamera, *mainLight);
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