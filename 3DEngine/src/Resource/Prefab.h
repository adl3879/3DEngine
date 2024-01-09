#pragma once

#include "Asset.h"
#include "Entity.h"
#include "Scene.h"

#include <memory>
#include <filesystem>

namespace Engine
{
class Prefab : public Asset
{
  public:
    Prefab();

    void CreateFromEntity(Entity entity);
    void AttachToScene(SceneRef scene);

    [[nodiscard]] SceneRef GetScene() const { return m_Scene; }
    [[nodiscard]] Entity GetRootEntity() const { return m_RootEntity; }
    [[nodiscard]] std::string GetDisplayName() const { return m_DisplayName; }
    std::vector<Entity> GetEntities() { return m_Entities; }

    [[nodiscard]] AssetType GetType() const override { return AssetType::Prefab; }

  private:
    void EntityWalker(Entity entity);

  private:
    std::string m_DisplayName;
    Entity m_RootEntity;
    SceneRef m_Scene;
    std::vector<Entity> m_Entities;
};

using PrefabRef = std::shared_ptr<Prefab>;
} // namespace Engine
