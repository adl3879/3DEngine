#pragma once

#include <filesystem>

#include "Entity.h"
#include "Scene.h"

namespace Engine
{
class PrefabSerializer
{
  public:
    PrefabSerializer(SceneRef scene) : m_Scene(scene) {}

    void Serialize(const std::filesystem::path &path, Entity e);
    Entity Deserialize(const std::filesystem::path &path);

  private:
	std::vector<Entity> FlattenEntity(Entity entity);

  private:
	SceneRef m_Scene;
};
} // namespace Engine
