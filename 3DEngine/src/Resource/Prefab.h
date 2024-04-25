#pragma once

#include "Asset.h"

namespace Engine
{
class Prefab : public Asset
{
  public:
    Prefab() = default;

	[[nodiscard]] AssetType GetType() const override { return AssetType::Prefab; }
};

using PrefabRef = std::shared_ptr<Prefab>;
}
