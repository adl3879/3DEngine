#pragma once

#include "Asset.h"
#include "AssetMetadata.h"

#include "Mesh.h"

namespace Engine
{
class MeshImporter
{
  public:
    static MeshRef ImportMesh(AssetHandle handle, const AssetMetadata &metadata);
};
} // namespace Engine