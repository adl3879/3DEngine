#pragma once

#include "Asset.h"
#include "AssetMetadata.h"

#include "TextureHDRI.h"
#include "Project.h"
#include "Log.h"

namespace Engine
{
class HDRIImporter
{
  public:
    static TextureHDRIRef ImportHDRI(AssetHandle handle, const AssetMetadata &metadata);
	static TextureHDRIRef LoadHDRI(const std::filesystem::path &path);
};
} // namespace Engine
