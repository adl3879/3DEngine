#pragma once

#include "AssetManagerBase.h"
#include "AssetMetadata.h"

#include <map>

namespace Engine
{
using AssetRegistry = std::map<AssetHandle, AssetMetadata>;

class EditorAssetManager : public AssetManagerBase
{
  public:
    virtual AssetRef GetAsset(AssetHandle handle) const override;

    virtual bool IsAssetHandleValid(AssetHandle handle) const override;
    virtual bool IsAssetLoaded(AssetHandle handle) const override;

    const AssetMetadata &GetMetadata(AssetHandle handle) const;

  private:
    AssetRegistry m_AssetRegistry;
    AssetMap m_LoadedAssets;

    // TODO: memory-only assets
};
} // namespace Engine