#pragma once

#include "UUID.h"

#include <memory>

namespace Engine
{
using AssetHandle = UUID;

enum class AssetType : uint16_t
{
    None = 0,
    Texture2D,
    Material,
    Mesh,
    Shader,
    Font,
    Sound,
    Script,
    Scene,
    Prefab,
};

class Asset
{
  public:
    AssetHandle Handle;

    virtual AssetType GetType() const = 0;
};

using AssetRef = std::shared_ptr<Asset>;
} // namespace Engine