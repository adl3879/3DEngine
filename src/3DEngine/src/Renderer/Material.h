#pragma once

#include <glm/glm.hpp>

#include <memory>
#include <array>
#include <string_view>

#include "Texture.h"
#include "Asset.h"

namespace Engine
{
struct MaterialData
{
    glm::vec3 Albedo;
    float AO;
    glm::vec3 Normal;
    float Metallic;
    float Roughness;
};

class Material
{
  public:
    enum ParameterType
    {
        ALBEDO = 0,
        AO,
        METALLIC,
        NORMAL,
        ROUGHNESS,
    };

    Material();

    void Init(const std::string &name, AssetHandle albedo, AssetHandle metallic, AssetHandle normal,
              AssetHandle roughness, AssetHandle alphaMask);

    void Init(const std::string_view name, const std::string_view albedoPath, const std::string_view aoPath,
              const std::string_view metallicPath, const std::string_view normalPath,
              const std::string_view roughnessPath, const std::string_view alphaMaskPath);

    void Init(const std::string_view name, const glm::vec3 &albedo, float ao, const glm::vec3 &normal, float metallic,
              float roughness, const float alpha = 1.0f);

    auto operator==(const Material &other) const noexcept { return Name == other.Name; }

    unsigned int GetParameterTexture(const ParameterType &type) const noexcept;
    Texture2DRef GetTexture(const ParameterType &type) const noexcept { return m_Textures[type]; }

    auto GetAlphaValue() const noexcept { return m_Alpha; }
    auto GetAlphaMask() const noexcept { return m_AlphaMaskTexture; }

    std::string_view Name;

  private:
    std::array<unsigned int, 5> m_MaterialTextures;
    std::array<Texture2DRef, 5> m_Textures;
    MaterialData m_Material;

    float m_Alpha;
    unsigned int m_AlphaMaskTexture;
};

using MaterialRef = std::shared_ptr<Material>;
} // namespace Engine