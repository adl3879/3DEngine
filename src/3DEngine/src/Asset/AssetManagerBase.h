#pragma once

#include "Asset.h"

#include <map>

namespace Engine
{
using AssetMap = std::map<AssetHandle, AssetRef>;

class AssetManagerBase
{

  public:
    virtual AssetRef GetAsset(AssetHandle handle) const = 0;

    virtual bool IsAssetHandleValid(AssetHandle handle) const = 0;
    virtual bool IsAssetLoaded(AssetHandle handle) const = 0;
};
} // namespace Engine