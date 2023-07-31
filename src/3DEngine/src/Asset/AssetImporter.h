#pragma once

#include "AssetMetadata.h"

namespace Engine
{
class AssetImporter
{
  public:
    static AssetRef ImportAsset(AssetHandle handle, const AssetMetadata &metadata);
};
} // namespace Engine