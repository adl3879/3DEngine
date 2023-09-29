#include "TransformSystem.h"

#include "Components.h"

namespace Engine
{
TransformSystem::TransformSystem(Scene *scene) { m_Scene = scene; }

void TransformSystem::Update(float dt) 
{ 
	auto view = m_Scene->GetRegistry().view<TransformComponent, ParentComponent>();
	for (auto entity : view)
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
	}
}
} // namespace Engine
