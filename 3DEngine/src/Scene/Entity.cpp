#include "Entity.h"

#include "Components.h"

namespace Engine
{
Entity::Entity(entt::entity handle, Scene *scene)
    : m_EntityHandle(handle), m_Scene(scene)
{
}

void Entity::AddChild(Entity &child)
{
    auto &parentComponent = child.GetComponent<ParentComponent>();
    parentComponent.HasParent = true;
    parentComponent.Parent = GetComponent<IDComponent>().ID;

    const auto id = child.GetComponent<IDComponent>().ID;
    GetComponent<ParentComponent>().Children.push_back(id);
    GetComponent<ParentComponent>().ChildEntities.push_back(child);
}

UUID Entity::GetUUID() { return GetComponent<IDComponent>().ID; }
} // namespace Engine
