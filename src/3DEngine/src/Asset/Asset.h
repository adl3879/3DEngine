#pragma once

#include "UUID.h"

#include <memory>
#include <string_view>

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

std::string_view AssetTypeToString(AssetType type);
AssetType AssetTypeFromString(std::string_view assetType);

class Asset
{
  public:
    AssetHandle Handle;

    virtual AssetType GetType() const = 0;
};

using AssetRef = std::shared_ptr<Asset>;
} // namespace Engine