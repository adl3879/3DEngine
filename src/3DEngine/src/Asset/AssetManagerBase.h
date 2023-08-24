#pragma once

#include "Asset.h"

#include <map>
#include <filesystem>

namespace Engine
{
using AssetMap = std::map<AssetHandle, AssetRef>;

class AssetManagerBase
{
  public:
    virtual AssetRef GetAsset(AssetHandle handle) = 0;
    virtual AssetRef GetAsset(const std::filesystem::path &path) = 0;
    virtual AssetHandle GetAssetHandleFromPath(const std::filesystem::path &path) = 0;
    virtual void UnloadAsset(AssetHandle handle) = 0;

    virtual bool IsAssetHandleValid(AssetHandle handle) const = 0;
    virtual bool IsAssetLoaded(AssetHandle handle) const = 0;
};
} // namespace Engine