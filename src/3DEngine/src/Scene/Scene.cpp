#include "Scene.h"

#include "Entity.h"
#include "Components.h"

namespace Engine
{
Scene::~Scene() {}

Entity Scene::CreateEntity(const std::string &name)
{
    Entity entity = {m_Registry.create(), this};
    auto tag = entity.AddComponent<TagComponent>();
    tag.Tag = name.empty() ? "Unnamed Entity" : name;
    entity.AddComponent<TransformComponent>();

    return entity;
}

void Scene::OnUpdate(float dt) {}

} // namespace Engine