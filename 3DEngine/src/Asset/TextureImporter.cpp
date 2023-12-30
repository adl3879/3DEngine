#include "TextureImporter.h"

#include <stb_image.h>

#include "Log.h"
#include "Project.h"

namespace Engine
{
Texture2DRef TextureImporter::ImportTexture2D(AssetHandle handle, const AssetMetadata &metadata)
{
    Texture2DRef tex = LoadTexture2D(Project::GetAssetDirectory() / metadata.FilePath);
    tex->Handle = handle;
    return tex;
}

Texture2DRef TextureImporter::LoadTexture2D(const std::filesystem::path &path)
{
    int width, height, channels;
    stbi_set_flip_vertically_on_load(1);
    Buffer data;
    {
        std::string pathStr = path.string();
        data.Data = stbi_load(pathStr.c_str(), &width, &height, &channels, 0);
    }

    if (data.Data == nullptr)
    {
        LOG_CORE_ERROR("TextureImporter::ImportTexture2D - Could not load texture from filepath: {}", path.string());
        return nullptr;
    }

    // TODO: think about this
    data.Size = width * height * channels;

    TextureSpecification spec;
    spec.Width = width;
    spec.Height = height;
    switch (channels)
    {
        case 1: spec.Format = ImageFormat::R8; break;
        case 3: spec.Format = ImageFormat::RGB8; break;
        case 4: spec.Format = ImageFormat::RGBA8; break;
        default: break;
    }

    Texture2DRef texture = std::make_shared<Texture2D>(spec, data);
    data.Release();
    return texture;
}
} // namespace Engine
