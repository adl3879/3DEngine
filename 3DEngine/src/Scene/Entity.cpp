#include "Entity.h"

#include "Components.h"

namespace Engine
{
Entity::Entity(entt::entity handle, Scene *scene) : m_EntityHandle(handle), m_Scene(scene) {}

void Entity::AddChild(Entity &child) 
{
    auto &parentComponent = child.GetComponent<ParentComponent>();
    parentComponent.HasParent = true;
	parentComponent.Parent = GetComponent<IDComponent>().ID;

	auto id = child.GetComponent<IDComponent>().ID;
	GetComponent<ParentComponent>().Children.push_back(id);
}
} // namespace Engine
