#pragma once

#include "Asset.h"
#include "AssetMetadata.h"
#include "NetScript.h"

namespace Engine
{
class NetScriptImporter
{
  public:
    static NetScriptRef ImportNetScript(AssetHandle handle, const AssetMetadata &metadata)
    {
        auto className = metadata.FilePath.stem().string();
        auto netScript = std::make_shared<NetScript>(Project::GetAssetDirectory() / metadata.FilePath, className);
        netScript->GenerateBoilerplate();
        netScript->Handle = handle;

        return netScript;
    }
};
} // namespace Engine