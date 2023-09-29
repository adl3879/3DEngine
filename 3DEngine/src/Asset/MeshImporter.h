#pragma once

#include "Asset.h"
#include "AssetMetadata.h"

#include "Model.h"
#include "Model.h"
#include "Project.h"

namespace Engine
{
class MeshImporter
{
  public:
    static ModelRef ImportMesh(AssetHandle handle, const AssetMetadata &metadata)
    {
        return LoadModel(Project::GetAssetDirectory() / metadata.FilePath);
    }

    // for loading models without any material information
    static ModelRef LoadModel(const std::filesystem::path &path) { return std::make_shared<Model>(path); }
};
} // namespace Engine
