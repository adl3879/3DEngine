#pragma once

#include "Asset.h"
#include "AssetMetadata.h"

#include "Model.h"

namespace Engine
{
class MeshImporter
{
  public:
    static ModelRef ImportMesh(AssetHandle handle, const AssetMetadata &metadata);
};
} // namespace Engine