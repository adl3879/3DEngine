#pragma once

#include "Asset.h"
#include "AssetMetadata.h"
#include "Project.h"

#include "Environment/SkyLight.h"

namespace Engine
{
class SkyLightImporter
{
  public:
    static SkyLightRef ImportSkyLight(AssetHandle handle, const AssetMetadata &metadata)
	{
        SkyLightRef skyLight = std::make_shared<SkyLight>(Project::GetAssetDirectory() / metadata.FilePath);
		skyLight->Init(2048);
        skyLight->Handle = handle;

		return skyLight;
	}
};
} // namespace Engine 
