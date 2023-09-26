#pragma once

#include <glm/glm.hpp>

#include <memory>
#include <array>
#include <string_view>
#include <any>
#include <filesystem>

#include "Texture2D.h"
#include "Asset.h"
#include "Shader.h"

namespace Engine
{
struct MaterialData
{
    glm::vec3 Albedo = glm::vec3(0.0f);
    float AO = 1.0f;
    glm::vec3 Normal = glm::vec3(0.0f);
    float Metallic = 0.0f;
    float Roughness = 0.9f;

	float Emissive = 1.0;
};

enum ParameterType
{
    ALBEDO = 0,
    METALLIC,
    NORMAL,
    ROUGHNESS,
    AO,
};

class Material : public Asset
{
  public:
    Material();

    void Init(const std::string &name, AssetHandle albedo, AssetHandle metallic, AssetHandle normal,
              AssetHandle roughness, AssetHandle ao);

    void Init(const std::string &name, const std::filesystem::path &albedoPath, const std::filesystem::path &aoPath,
              const std::filesystem::path &metallicPath, const std::filesystem::path &normalPath,
              const std::filesystem::path &roughnessPath, const std::filesystem::path &alphaMaskPath);

    void Init(const std::string_view name, const glm::vec3 &albedo, float ao, const glm::vec3 &normal, float metallic,
              float roughness, const float alpha = 1.0f);

    auto operator==(const Material &other) const noexcept { return Name == other.Name; }

    unsigned int GetParameterTexture(const ParameterType &type) const noexcept;
    Texture2DRef GetTexture(const ParameterType &type) const noexcept { return m_Textures[type]; }

	bool Reset(ParameterType type);

    void Bind(Shader *shader) const noexcept;
    void Unbind() const noexcept;

    virtual AssetType GetType() const override { return AssetType::Material; }

  public:
    MaterialData GetMaterialData() const { return m_MaterialParam; }
    bool GetUseNormalMap() const { return m_UseNormalMap; }
    std::array<Texture2DRef, 5> GetTextures() const { return m_Textures; }
    std::array<AssetHandle, 5> GetTextureHandles() const { return m_TextureHandles; }

	int HasMaterialMap(ParameterType type) const;

  public:
    void SetTexture(ParameterType type, AssetHandle textureHandle);
    void SetMaterialParam(ParameterType type, std::any param);
    void SetUseNormalMap(bool useNormalMap) { m_UseNormalMap = useNormalMap; }
    void SetEmissiveValue(float value) { m_MaterialParam.Emissive = value; }

  public:
    std::string Name;
	bool IsDefault = false;

  private:
    MaterialData m_MaterialParam;
    bool m_UseNormalMap = true;

  private:
    std::array<unsigned int, 5> m_MaterialTextures;
    std::array<Texture2DRef, 5> m_Textures;
    std::array<AssetHandle, 5> m_TextureHandles;
};

using MaterialRef = std::shared_ptr<Material>;
} // namespace Engine
