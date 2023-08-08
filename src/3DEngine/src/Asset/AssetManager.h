#pragma once

#include "AssetManagerBase.h"
#include "Project.h"

#include <memory>
#include <filesystem>

namespace Engine
{
class AssetManager
{
  public:
    static AssetRegistry GetRegistry() { return Project::GetActive()->GetEditorAssetManager()->GetAssetRegistry(); }

    template <typename T> static std::shared_ptr<T> GetAsset(AssetHandle handle)
    {
        AssetRef asset = Project::GetActive()->GetAssetManager()->GetAsset(handle);
        return std::static_pointer_cast<T>(asset);
    }

    static AssetHandle GetAssetHandleFromPath(const std::filesystem::path &path)
    {
        return Project::GetActive()->GetEditorAssetManager()->GetAssetHandleFromPath(path);
    }

    static void UnloadAsset(AssetHandle handle) { Project::GetActive()->GetEditorAssetManager()->UnloadAsset(handle); }

    static AssetHandle ImportAsset(const std::filesystem::path &path)
    {
        return Project::GetActive()->GetEditorAssetManager()->ImportAsset(path);
    }
};
} // namespace Engine