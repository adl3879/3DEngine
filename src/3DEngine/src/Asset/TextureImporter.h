#pragma once

#include "Asset.h"
#include "AssetMetadata.h"

#include "Texture.h"

namespace Engine
{

class TextureImporter
{
  public:
    static Texture2DRef ImportTexture2D(AssetHandle handle, const AssetMetadata &metadata);
    static Texture2DRef LoadTexture2D(const std::filesystem::path &path);
};
} // namespace Engine