#include "TransformSystem.h"

#include "Components.h"

namespace Engine
{
TransformSystem::TransformSystem(Scene *scene) { m_Scene = scene; }

void TransformSystem::Update(float dt)
{
    auto view = m_Scene->GetRegistry().view<TransformComponent, ParentComponent>();
    for (const auto entity : view)
    {
        Entity ent = Entity{entity, m_Scene};
        auto &transformComponent = ent.GetComponent<TransformComponent>();
        auto &parentComponent = ent.GetComponent<ParentComponent>();

        if (parentComponent.HasParent)
        {
            auto parent = m_Scene->GetEntityByUUID(parentComponent.Parent);
            const auto &parentTransform = parent.GetComponent<TransformComponent>();

            transformComponent.Translation = parentTransform.Translation + transformComponent.LocalTranslation;
            transformComponent.Rotation = parentTransform.Rotation * transformComponent.LocalRotation;
            transformComponent.Scale = parentTransform.Scale * transformComponent.LocalScale;

            transformComponent.SetTransform(parentTransform.GetTransform() * transformComponent.GetLocalTransform());
        }

        // camera
        auto camView = m_Scene->GetRegistry().view<TransformComponent, CameraComponent>();
        for (auto &e : camView)
        {
            auto [camera, transform] = camView.get<CameraComponent, TransformComponent>(e);
            camera.Camera->SetPosition(transform.Translation);
            camera.Camera->SetRotation(transform.Rotation);
        }
    }
}
} // namespace Engine
