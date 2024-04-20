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
    parentComponent.Parent = this->GetComponent<IDComponent>().ID;

    const auto id = child.GetComponent<IDComponent>().ID;
    this->GetComponent<ParentComponent>().Children.push_back(id);
}

UUID Entity::GetUUID() { return this->GetComponent<IDComponent>().ID; }
} // namespace Engine
