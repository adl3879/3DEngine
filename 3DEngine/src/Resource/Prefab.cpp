#include "Prefab.h"

#include "SceneSerializer.h"
#include "Components.h"
#include "Log.h"

#include <fstream>

namespace Engine
{
Prefab::Prefab() { m_Scene = std::make_shared<Scene>(); }

void Prefab::CreateFromEntity(Entity entity)
{
    m_RootEntity = entity;
    m_DisplayName = entity.GetComponent<TagComponent>().Tag;

    auto e = m_Scene->DuplicateEntityRecursive(entity);
    EntityWalker(e);
}

void Prefab::EntityWalker(Entity entity)
{
    entity.GetComponent<TagComponent>().IsPrefab = true;

    for (auto e : entity.GetComponent<ParentComponent>().ChildEntities) EntityWalker(e);
}

void Prefab::AttachToScene(SceneRef scene)
{
    LOG_CORE_INFO("Attaching prefab to scene");
    scene->Merge(m_Scene);
}
} // namespace Engine