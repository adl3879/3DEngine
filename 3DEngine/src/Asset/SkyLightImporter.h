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
		SkyLightRef skyLight = std::make_shared<SkyLight>();
		skyLight->Init(Project::GetAssetDirectory() / metadata.FilePath, 2048);

		return skyLight;
	}
};
} // namespace Engine
