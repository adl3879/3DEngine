#include "TransformSystem.h"

#include "Components.h"

namespace Engine
{
TransformSystem::TransformSystem(Scene *scene) { m_Scene = scene; }

bool TransformSystem::Init()
{
    UpdateTransform();
    return true;
}

void TransformSystem::Update(float dt) { UpdateTransform(); }

void TransformSystem::UpdateTransform()
{
    // Calculate all local transforms
    auto localTransformView = m_Scene->GetRegistry().view<TransformComponent>();
    for (auto tv : localTransformView)
    {
        TransformComponent &transform = localTransformView.get<TransformComponent>(tv);
        Entity currentEntity = {tv, m_Scene};
        ParentComponent &parentComponent = currentEntity.GetComponent<ParentComponent>();
        if (transform.Dirty)
        {
            const glm::vec3 &localTranslate = transform.GetLocalPosition();
            const glm::quat &localRot = glm::normalize(transform.GetLocalRotation());
            const glm::vec3 &localScale = transform.GetLocalScale();
            const glm::mat4 &translationMatrix = glm::translate(glm::mat4(1.0f), localTranslate);
            const glm::mat4 &rotationMatrix = glm::mat4_cast(localRot);
            const glm::mat4 &scaleMatrix = glm::scale(glm::mat4(1.0f), localScale);
            const glm::mat4 &newLocalTransform = translationMatrix * rotationMatrix * scaleMatrix;

            transform.GlobalDirty = true;
            UpdateDirtyFlagRecursive(currentEntity);

            transform.SetLocalTransform(newLocalTransform);
            transform.Dirty = false;
        }
    }

    // Calculate all global transforms
    auto transformView = m_Scene->GetRegistry().view<ParentComponent, TransformComponent>();
    for (auto e : transformView)
    {
        auto [parent, transform] = transformView.get<ParentComponent, TransformComponent>(e);
        if (!parent.HasParent)
        {
            // If no parents, then globalTransform is local transform.
            transform.SetGlobalTransform(transform.GetLocalTransform());
            transform.SetGlobalPosition(transform.GetLocalPosition());
            transform.SetGlobalRotation(transform.GetLocalRotation());
            transform.SetGlobalScale(transform.GetLocalScale());
            continue;
        }

        Entity currentParent = Entity((entt::entity)e, m_Scene);

        glm::mat4 globalTransform = transform.GetLocalTransform();
        glm::vec3 globalPosition = transform.GetLocalPosition();
        glm::quat globalOrientation = transform.GetLocalRotation();
        glm::vec3 globalScale = transform.GetLocalScale();

        ParentComponent parentComponent = currentParent.GetComponent<ParentComponent>();
#define FRAME_PERFECT_TRANSFORM
#ifndef FRAME_PERFECT_TRANSFORM
        if (parentComponent.HasParent)
        {
            TransformComponent &transformComponent = parentComponent.Parent.GetComponent<TransformComponent>();

            globalPosition = transformComponent.GetGlobalPosition() + (globalPosition);

            globalScale *= transformComponent.GetGlobalScale();
            globalOrientation = transformComponent.GetGlobalRotation() * globalOrientation;
            globalTransform = transformComponent.GetGlobalTransform() * globalTransform;
        }
#else
        bool exitEarly = false;
        while (parentComponent.HasParent)
        {
			auto parent = m_Scene->GetEntityByUUID(parentComponent.Parent);
			//auto tag = parent.GetComponent<TagComponent>().Tag;

            TransformComponent &transformComponent = parent.GetComponent<TransformComponent>();

            globalPosition = transformComponent.GetLocalPosition() + (globalPosition);
            globalScale *= transformComponent.GetLocalScale();
            globalOrientation = transformComponent.GetLocalRotation() * globalOrientation;
            globalTransform = transformComponent.GetLocalTransform() * globalTransform;
            transformComponent.GlobalDirty = false;

            parentComponent = parent.GetComponent<ParentComponent>();
        }
#endif // FRAME_PERFECT_TRANSFORM

        transform.SetGlobalPosition(globalPosition);
        transform.SetGlobalRotation(globalOrientation);
        transform.SetGlobalScale(globalScale);
        transform.SetGlobalTransform(globalTransform);
    }

    auto camView = m_Scene->GetRegistry().view<TransformComponent, CameraComponent>();
    for (auto &e : camView)
    {
        auto [transform, camera] = camView.get<TransformComponent, CameraComponent>(e);

        camera.Camera->SetPosition(transform.Translation);
		camera.Camera->SetRotation(transform.Rotation);

        const glm::quat &globalRotation = transform.GetGlobalRotation();
        const glm::mat4 &translationMatrix = glm::translate(glm::mat4(1.0f), transform.GetGlobalPosition());
        const glm::mat4 &rotationMatrix = glm::mat4_cast(globalRotation);
        const glm::vec4 &forward = glm::vec4(0, 0, -1, 1);
        const glm::vec4 &globalForward = rotationMatrix * forward;

        /*const glm::vec4 &right = glm::vec4(1, 0, 0, 1);
        const glm::vec4 &globalRight = rotationMatrix * right;
        camera.CameraInstance->Direction = globalForward;
        camera.CameraInstance->Right = globalRight;*/
        
        //camera.Camera->SetTransform(glm::inverse(translationMatrix * rotationMatrix));
    }
}

void TransformSystem::UpdateDirtyFlagRecursive(Entity &entity)
{
    auto &parentComponent = entity.GetComponent<ParentComponent>();

    for (auto &c : parentComponent.Children)
    {
		auto child = m_Scene->GetEntityByUUID(c);
        auto &childParentComponent = child.GetComponent<TransformComponent>();
        childParentComponent.GlobalDirty = true;

        UpdateDirtyFlagRecursive(child);
    }
}

void TransformSystem::CalculateGlobalTransform(Entity &entity)
{
    auto &parentComponent = entity.GetComponent<ParentComponent>();
    auto &transformComponent = entity.GetComponent<TransformComponent>();
    auto &parentTransformComponent = m_Scene->GetEntityByUUID(parentComponent.Parent).GetComponent<TransformComponent>();

    glm::vec3 globalPosition = parentTransformComponent.GetGlobalPosition() + glm::vec3(transformComponent.GetLocalPosition());
    glm::quat globalRotation = parentTransformComponent.GetGlobalRotation() * transformComponent.GetLocalRotation();
    glm::vec3 globalScale = parentTransformComponent.GetGlobalScale() * transformComponent.GetGlobalScale();
    auto globalTransform = transformComponent.GetGlobalTransform() * parentTransformComponent.GetGlobalTransform();

    transformComponent.SetGlobalPosition(glm::vec3(globalPosition));
    transformComponent.SetGlobalRotation(globalRotation);
    transformComponent.SetGlobalScale(globalScale);
    transformComponent.SetGlobalTransform(globalTransform);
    transformComponent.GlobalDirty = false;

    for (auto &c : parentComponent.Children)
    {
		auto child = m_Scene->GetEntityByUUID(c);
        CalculateGlobalTransform(child);
    }
}
} // namespace Engine
