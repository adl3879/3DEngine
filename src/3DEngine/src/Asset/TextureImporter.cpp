#include "TextureImporter.h"

#include <stb_image.h>

#include "Log.h"

namespace Engine
{
Texture2DRef TextureImporter::ImportTexture2D(AssetHandle handle, const AssetMetadata &metadata)
{
    int width, height, channels;
    stbi_set_flip_vertically_on_load(1);
    Buffer data;

    {
        std::string pathStr = metadata.FilePath.string();
        data.Data = stbi_load(pathStr.c_str(), &width, &height, &channels, 0);
    }

    if (data.Data == nullptr)
    {
        LOG_CORE_ERROR("TextureImporter::ImportTexture2D - Could not load texture from filepath: {}",
                       metadata.FilePath.string());
        return nullptr;
    }

    // TODO: think about this
    data.Size = width * height * channels;

    TextureSpecification spec;
    spec.Width = width;
    spec.Height = height;
    switch (channels)
    {
        case 3: spec.Format = ImageFormat::RGB8; break;
        case 4: spec.Format = ImageFormat::RGBA8; break;
    }

    Texture2DRef texture = std::make_shared<Texture2D>(spec, data);
    data.Release();
    return texture;
}
} // namespace Engine