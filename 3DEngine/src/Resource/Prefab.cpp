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

    EntityWalker(entity);
}

void Prefab::EntityWalker(Entity entity)
{
    m_Entities.push_back(entity);

    for (const auto &e : entity.GetComponent<ParentComponent>().ChildEntities) EntityWalker(e);
}

void Prefab::AttachToScene(SceneRef scene)
{
    LOG_CORE_INFO("Attaching prefab to scene");
    scene->Merge(m_Scene);
}
} // namespace Engine
