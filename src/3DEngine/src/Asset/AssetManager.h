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
    template <typename T> static std::shared_ptr<T> GetAsset(AssetHandle handle)
    {
        AssetRef asset = Project::GetActive()->GetAssetManager()->GetAsset(handle);
        return std::static_pointer_cast<T>(asset);
    }

    static AssetHandle ImportAsset(const std::filesystem::path &path)
    {
        return Project::GetActive()->GetEditorAssetManager()->ImportAsset(path);
    }
};
} // namespace Engine