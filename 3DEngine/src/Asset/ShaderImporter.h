#pragma once

#include "Asset.h"
#include "AssetMetadata.h"

#include "CustomShader.h"
#include "Project.h"

namespace Engine
{
class ShaderImporter
{
public:
    static CustomShaderRef ImportShader(AssetHandle handle, const AssetMetadata &metadata)
    {
        auto shader = std::make_shared<CustomShader>(Project::GetAssetDirectory() / metadata.FilePath);
        return shader;
    }
};
} // namespace Engine
