#pragma once

#include "Asset.h"
#include "AssetMetadata.h"
#include "AssetManager.h"

#include "Model.h"
#include "Model.h"
#include "Project.h"
#include "Log.h"
#include "Misc/MeshFileManager.h"
#include "MaterialSerializer.h"
#include "Entity.h"
#include "Components.h"
#include "Prefab.h"
#include "PrefabSerializer.h"

#include <filesystem>

namespace Engine
{
class MeshImporter
{
  public:
    static ModelRef ImportMesh(AssetHandle handle, const AssetMetadata &metadata);
    static bool LoadModel(const std::filesystem::path &path, const std::filesystem::path &currentDir);
    static bool ReloadModel(const std::filesystem::path &path, const std::filesystem::path &to);
};
} // namespace Engine
