#include "EditorAssetManager.h"

#include "AssetImporter.h"
#include "Log.h"

namespace Engine
{
bool EditorAssetManager::IsAssetHandleValid(AssetHandle handle) const
{
    return handle != 0 && m_AssetRegistry.find(handle) != m_AssetRegistry.end();
}

bool EditorAssetManager::IsAssetLoaded(AssetHandle handle) const
{
    return m_LoadedAssets.find(handle) != m_LoadedAssets.end();
}

const AssetMetadata &EditorAssetManager::GetMetadata(AssetHandle handle) const
{
    static AssetMetadata s_NullMetadata;
    auto it = m_AssetRegistry.find(handle);
    if (it == m_AssetRegistry.end()) return s_NullMetadata;

    return it->second;
}

AssetRef EditorAssetManager::GetAsset(AssetHandle handle) const
{
    // 1. check if handle is valid
    if (!IsAssetHandleValid(handle)) return nullptr;

    // 2. check if asset needs load (and if so, load)
    AssetRef asset;
    if (IsAssetLoaded(handle))
    {
        asset = m_LoadedAssets.at(handle);
    }
    else
    {
        // load asset
        const AssetMetadata &metadata = GetMetadata(handle);
        asset = AssetImporter::ImportAsset(handle, metadata);
        if (!asset)
        {
            LOG_CORE_ERROR("EditorAssetManager::GetAsset - asset import failed!");
        }
    }
    // 3. return asset
    return asset;
}
} // namespace Engine