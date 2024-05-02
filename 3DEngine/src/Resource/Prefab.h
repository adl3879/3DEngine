#pragma once

#include "Asset.h"
#include "Scene.h"
#include "Entity.h"

namespace Engine
{
class Entity;
class Prefab : public Asset
{
  public:
    Prefab() = default;

	void Apply(const SceneRef &scene, Entity &entity);
	void Revert(const SceneRef &scene, Entity &entity) const;

	[[nodiscard]] AssetType GetType() const override { return AssetType::Prefab; }
};

using PrefabRef = std::shared_ptr<Prefab>;
}
