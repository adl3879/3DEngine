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
        return std::make_shared<Model>(Project::GetAssetDirectory() / metadata.FilePath);
    }
};
} // namespace Engine