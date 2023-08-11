#pragma once

#include <glm/glm.hpp>

#include <memory>
#include <array>
#include <string_view>
#include <any>
#include <filesystem>

#include "Texture.h"
#include "Asset.h"

namespace Engine
{
struct MaterialData
{
    glm::vec3 Albedo = glm::vec3(0.0f);
    float AO = 0.0f;
    glm::vec3 Normal = glm::vec3(0.0f);
    float Metallic = 0.5f;
    float Roughness = 0.5f;
};

enum ParameterType
{
    ALBEDO = 0,
    AO,
    METALLIC,
    NORMAL,
    ROUGHNESS,
};

class Material : public Asset
{
  public:
    Material();

    void Init(const std::string &name, AssetHandle albedo, AssetHandle metallic, AssetHandle normal,
              AssetHandle roughness);

    void Init(const std::string_view name, const std::string_view albedoPath, const std::string_view aoPath,
              const std::string_view metallicPath, const std::string_view normalPath,
              const std::string_view roughnessPath, const std::string_view alphaMaskPath);

    void Init(const std::string_view name, const glm::vec3 &albedo, float ao, const glm::vec3 &normal, float metallic,
              float roughness, const float alpha = 1.0f);

    auto operator==(const Material &other) const noexcept { return Name == other.Name; }

    unsigned int GetParameterTexture(const ParameterType &type) const noexcept;
    Texture2DRef GetTexture(const ParameterType &type) const noexcept { return m_Textures[type]; }
    void BindMaterialTextures(uint32_t startIndex) const noexcept;
    void UnbindMaterialTextures() const noexcept;

    auto GetAlphaValue() const noexcept { return m_Alpha; }
    auto GetAlphaMask() const noexcept { return m_AlphaMaskTexture; }

    virtual AssetType GetType() const override { return AssetType::Material; }

  public:
    MaterialData GetMaterialData() const { return m_MaterialParam; }
    bool GetUseNormalMap() const { return m_UseNormalMap; }
    std::array<Texture2DRef, 5> GetTextures() const { return m_Textures; }
    std::array<AssetHandle, 5> GetTextureHandles() const { return m_TextureHandles; }

  public:
    void SetTexture(ParameterType type, AssetHandle textureHandle);
    void SetMaterialParam(ParameterType type, std::any param);
    void SetUseNormalMap(bool useNormalMap) { m_UseNormalMap = useNormalMap; }

  public:
    std::string Name;

  private:
    MaterialData m_MaterialParam;
    bool m_UseNormalMap = true;

  private:
    std::array<unsigned int, 5> m_MaterialTextures;
    std::array<Texture2DRef, 5> m_Textures;
    std::array<AssetHandle, 5> m_TextureHandles;

    float m_Alpha;
    unsigned int m_AlphaMaskTexture;

    std::filesystem::path m_MaterialPath;
};

using MaterialRef = std::shared_ptr<Material>;
} // namespace Engine