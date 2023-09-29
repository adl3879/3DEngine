#pragma once

#include "AssetManagerBase.h"
#include "AssetMetadata.h"
#include "Asset.h"

#include <map>

namespace Engine
{
using AssetRegistry = std::map<AssetHandle, AssetMetadata>;

class EditorAssetManager : public AssetManagerBase
{
  public:
    virtual AssetRef GetAsset(AssetHandle handle) override;
    virtual AssetRef GetAsset(const std::filesystem::path &path) override;
    virtual AssetHandle GetAssetHandleFromPath(const std::filesystem::path &path) override;
    virtual void UnloadAsset(AssetHandle handle) override;
    virtual std::string GetAssetName(AssetHandle handle) const override;

    virtual bool IsAssetHandleValid(AssetHandle handle) const override;
    virtual bool IsAssetLoaded(AssetHandle handle) const override;

    AssetHandle ImportAsset(const std::filesystem::path &path);
    AssetHandle AddAsset(AssetRef asset);

    const AssetMetadata &GetMetadata(AssetHandle handle) const;
    const AssetRegistry &GetAssetRegistry() const { return m_AssetRegistry; }

    void SerializeAssetRegistry();
    bool DeserializeAssetRegistry();

  private:
    AssetRegistry m_AssetRegistry;
    AssetMap m_LoadedAssets;

    // TODO: memory-only assets
};
} // namespace Engine
