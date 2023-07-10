#include "ResourceManager.h"

#include <glad/glad.h>
#include <stb_image.h>

#include <fstream>
#include <sstream>

#include "Log.h"

namespace Engine
{
using ImageBuffer = std::unique_ptr<unsigned char[]>;

struct CompressedImageDesc
{
    GLint Width{-1};
    GLint Height{-1};
    GLint Size{-1};
    GLint Format{-1};
    ImageBuffer Data;
};

const static fs::path COMPRESSED_TEXT_DIR{fs::current_path() / "Data/cache/textures"};

static auto buildTextureCachePath(const fs::path &filenameNoExt)
{
    const fs::path filename{filenameNoExt.string() + ".bin"};
    return fs::path(COMPRESSED_TEXT_DIR / filename);
}

static std::optional<CompressedImageDesc> loadCompressedImageFromDisk(const fs::path &target)
{
    CompressedImageDesc desc;

    std::ifstream in(target, std::ios::binary);
    if (!in) return std::nullopt;

    in.read(reinterpret_cast<char *>(&desc.Size), sizeof(CompressedImageDesc::Size));
    in.read(reinterpret_cast<char *>(&desc.Width), sizeof(CompressedImageDesc::Width));
    in.read(reinterpret_cast<char *>(&desc.Height), sizeof(CompressedImageDesc::Height));
    in.read(reinterpret_cast<char *>(&desc.Format), sizeof(CompressedImageDesc::Format));

    desc.Data = std::make_unique<unsigned char[]>(desc.Size);
    in.read(reinterpret_cast<char *>(desc.Data.get()), desc.Size);

    return std::make_optional(std::move(desc));
}

static void saveCompressedImageToDisk(const fs::path &target, const CompressedImageDesc &desc)
{
    if (!fs::exists(COMPRESSED_TEXT_DIR))
    {
        if (!fs::create_directories(COMPRESSED_TEXT_DIR))
        {
            LOG_CORE_ERROR("Failed to create texture cache directory: {}", COMPRESSED_TEXT_DIR.string());
            return;
        }
    }

    std::ofstream out(target, std::ios::binary);
    if (out)
    {
        out.write((char *)&desc.Size, sizeof(CompressedImageDesc::Size));
        out.write((char *)&desc.Width, sizeof(CompressedImageDesc::Width));
        out.write((char *)&desc.Height, sizeof(CompressedImageDesc::Height));
        out.write((char *)&desc.Format, sizeof(CompressedImageDesc::Format));
        out.write((char *)desc.Data.get(), desc.Size);
    }
}

unsigned int ResourceManager::LoadTexture(const fs::path &path, const bool useMipMaps, const bool useUnalignedUnpack)
{
    if (path.filename().empty()) return 0;

    const auto compressedFilePath{buildTextureCachePath(path.stem())};
    const auto compressedImageExists{fs::exists(compressedFilePath)};

    const fs::path pathToLoad = compressedImageExists ? compressedFilePath : path;

    // check if texture is already loaded in memory
    if (const auto val = m_TextureCache.find(pathToLoad); val != m_TextureCache.end())
    {
        // found it
        return val->second;
    }

    if (useUnalignedUnpack) glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    unsigned int textureID;
    glGenTextures(1, &textureID);

    if (compressedImageExists)
    {
        const auto desc{loadCompressedImageFromDisk(compressedFilePath)};
        if (!desc) return 0;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glCompressedTexImage2D(GL_TEXTURE_2D, 0, desc.value().Format, desc.value().Width, desc.value().Height, 0,
                               desc.value().Size, desc.value().Data.get());
    }
    else
    {
        int width = 0, height = 0, nrComponents = 0;
        unsigned char *data = stbi_load(pathToLoad.c_str(), &width, &height, &nrComponents, 0);
        if (!data)
        {
            LOG_CORE_ERROR("Failed to load texture: {}", pathToLoad.c_str());
            glDeleteTextures(1, &textureID);
            stbi_image_free(data);
            return 0;
        }

        uint32_t format = 0;
        uint32_t internalFormat = 0;
        switch (nrComponents)
        {
            case 1:
            {
                format = GL_RED;
                internalFormat = GL_RED;
                break;
            }
            case 3:
            {
                format = GL_RGB;
                internalFormat = GL_RGB8;
                break;
            }
            case 4:
            {
                format = GL_RGBA;
                internalFormat = GL_RGBA8;
                break;
            }
        }

        glBindTexture(GL_TEXTURE_2D, textureID);

        glHint(GL_TEXTURE_COMPRESSION_HINT, GL_DONT_CARE);
        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, data);

        stbi_image_free(data);

        GLint compressed = GL_FALSE;
        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_COMPRESSED, &compressed);
        if (compressed == GL_TRUE)
        {
            CompressedImageDesc desc{
                .Width = width,
                .Height = height,
            };

            glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_INTERNAL_FORMAT, &desc.Size);
            glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_INTERNAL_FORMAT, &desc.Format);

            desc.Data = std::make_unique<unsigned char[]>(desc.Size);
            glGetCompressedTexImage(GL_TEXTURE_2D, 0, desc.Data.get());
        }
    }

    if (useMipMaps) glGenerateMipmap(GL_TEXTURE_2D);

    if (useUnalignedUnpack) glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

    return m_TextureCache.try_emplace(pathToLoad, textureID).first->second;
}

std::optional<MaterialPtr> ResourceManager::GetMaterial(const std::string_view name) const
{
    // check if material exists
    const auto val = m_MaterialCache.find(name.data());

    if (val == m_MaterialCache.end()) return std::optional<MaterialPtr>();

    return std::make_optional<MaterialPtr>(val->second);
}

MaterialPtr ResourceManager::CacheMaterial(const std::string_view name, const std::string_view albedoPath,
                                           const std::string_view aoPath, const std::string_view metallicPath,
                                           const std::string_view normalPath, const std::string_view roughnessPath,
                                           const std::string_view alphaMaskPath)
{
    auto material = Material();
    material.Init(name, albedoPath, aoPath, metallicPath, normalPath, roughnessPath, alphaMaskPath);

    return m_MaterialCache.try_emplace(name.data(), std::make_shared<Material>(material)).first->second;
}
} // namespace Engine
