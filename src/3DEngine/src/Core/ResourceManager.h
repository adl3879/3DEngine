#pragma once

#include <unordered_map>
#include <optional>
#include <filesystem>

#include "Material.h"

namespace Engine
{
namespace fs = std::filesystem;

class ResourceManager
{
  public:
    static ResourceManager &Instance()
    {
        static ResourceManager instance;
        return instance;
    }

    ResourceManager(const ResourceManager &) = delete;
    ResourceManager &operator=(const ResourceManager &) = delete;

  public:
    // loads a texture from file if not cached
    unsigned int LoadTexture(const fs::path &path, const bool useMipMaps = true, const bool useUnalignedUnpack = false);
    unsigned int LoadHDRI(const std::string &path) const;

    std::optional<MaterialPtr> GetMaterial(const std::string_view name) const;
    MaterialPtr CacheMaterial(const std::string_view name, const std::string_view albedoPath,
                              const std::string_view aoPath, const std::string_view metallicPath,
                              const std::string_view normalPath, const std::string_view roughnessPath,
                              const std::string_view alphaMaskPath);

    auto GetNumLoadedTextures() const noexcept { return m_TextureCache.size(); }

  private:
    ResourceManager() = default;
    ~ResourceManager() = default;

  private:
    std::unordered_map<std::string, unsigned int> m_TextureCache;
    std::unordered_map<std::string, MaterialPtr> m_MaterialCache;
};
} // namespace Engine