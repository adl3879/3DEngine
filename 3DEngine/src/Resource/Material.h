#pragma once

#include <glm/glm.hpp>

#include <memory>
#include <array>
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
    NORMAL,
    METALLIC,
    ROUGHNESS,
    AO,
};

class Material : public Asset
{
public:
    Material();

    void Init(const std::string &name, AssetHandle albedo, AssetHandle normal, AssetHandle metallic,
              AssetHandle roughness, AssetHandle ao);

    void Init(const std::string &name, const std::string &albedo, const std::string &normal,
              const std::string &metallic, const std::string &roughness, const std::string &ao);

    void Init(std::string_view name, const glm::vec3 &albedo, const glm::vec3 &normal, float metallic, float roughness,
              float ao, float alpha = 1.0f);

    auto operator==(const Material &other) const noexcept { return this->Handle == other.Handle; }
    auto operator<(const Material &other) const noexcept { return this->Handle < other.Handle; }

    bool Reset(ParameterType type);

    void Bind(Shader *shader) noexcept;
    void Unbind() const noexcept;

    [[nodiscard]] AssetType GetType() const override { return AssetType::Material; }

    [[nodiscard]] MaterialData GetMaterialData() const { return m_MaterialParam; }
    [[nodiscard]] std::array<AssetHandle, 5> GetTextureHandles() const { return m_TextureHandles; }
    [[nodiscard]] std::string GetTexturePath(ParameterType type) const;
    [[nodiscard]] Texture2DRef GetTexture(ParameterType type) const;

    [[nodiscard]] bool GetUseNormalMap() const { return m_UseNormalMap; }
    [[nodiscard]] int HasMaterialMap(ParameterType type) const;
    [[nodiscard]] AssetHandle GetShaderHandle() const { return m_ShaderHandle; }

    void SetTexture(ParameterType type, AssetHandle textureHandle);
    void SetTexturePath(ParameterType type, const std::string &path);
    void SetMaterialParam(ParameterType type, std::any param);
    void SetUseNormalMap(bool useNormalMap) { m_UseNormalMap = useNormalMap; }
    void SetEmissiveValue(float value) { m_MaterialParam.Emissive = value; }
    void SetShaderHandle(AssetHandle shaderHandle) { m_ShaderHandle = shaderHandle; }

	void SetTextureFindPath(const std::filesystem::path &path) { m_TextureFindPath = path; }

private:
    void BindTextures();

public:
    std::string Name;
    bool IsDefault = false;

private:
    MaterialData m_MaterialParam;
    bool m_UseNormalMap = true;
    AssetHandle m_ShaderHandle = 0;

	std::filesystem::path m_TextureFindPath;

    std::array<AssetHandle, 5> m_TextureHandles;
    std::array<std::string, 5> m_TexturePaths;
};

using MaterialRef = std::shared_ptr<Material>;
} // namespace Engine
