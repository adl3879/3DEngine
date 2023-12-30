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
    AssetRef GetAsset(AssetHandle handle) override;
    AssetRef GetAsset(const std::filesystem::path &path) override;
    AssetHandle GetAssetHandleFromPath(const std::filesystem::path &path) override;
    void UnloadAsset(AssetHandle handle) override;
    [[nodiscard]] std::string GetAssetName(AssetHandle handle) const override;

    [[nodiscard]] bool IsAssetHandleValid(AssetHandle handle) const override;
    [[nodiscard]] bool IsAssetLoaded(AssetHandle handle) const override;

    AssetHandle ImportAsset(const std::filesystem::path &path);
    AssetHandle AddAsset(AssetRef asset, const std::filesystem::path &path = "");

    [[nodiscard]] const AssetMetadata &GetMetadata(AssetHandle handle) const;
    [[nodiscard]] const AssetRegistry &GetAssetRegistry() const { return m_AssetRegistry; }

    void SerializeAssetRegistry();
    bool DeserializeAssetRegistry();

  private:
    AssetRegistry m_AssetRegistry;
    AssetMap m_LoadedAssets;

    // TODO: memory-only assets
};
} // namespace Engine
