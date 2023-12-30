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
    TextureHDRI,
    Material,
    Mesh,
	SkinnedMesh,
    Shader,
    Font,
    Sound,
    Script,
    LuaScript,
    NetScript,
    Scene,
    Prefab,
    SkyLight,
    Folder,
};

std::string_view AssetTypeToString(AssetType type);
AssetType AssetTypeFromString(std::string_view assetType);

class Asset
{
public:
    AssetHandle Handle;

    [[nodiscard]] virtual AssetType GetType() const = 0;
};

using AssetRef = std::shared_ptr<Asset>;
} // namespace Engine
