#pragma once

#include "Asset.h"
#include "AssetMetadata.h"
#include "Prefab.h"
#include "PrefabSerializer.h"
#include "Project.h"

#include <filesystem>

namespace Engine
{
class PrefabImporter
{
  public:
    static PrefabRef ImportPrefab(AssetHandle handle, const AssetMetadata &metadata)
    {
        auto prefab = std::make_shared<Prefab>();
        PrefabSerializer serializer(prefab);
        serializer.Deserialize(Project::GetAssetDirectory() / metadata.FilePath);

        return prefab;
    }
};
} // namespace Engine