#include "Material.h"

#include "AssetManager.h"
#include "Texture.h"
#include "MaterialSerializer.h"

namespace Engine
{
Material::Material()
{
    std::fill(m_MaterialTextures.begin(), m_MaterialTextures.end(), 0);
    std::fill(m_TextureHandles.begin(), m_TextureHandles.end(), 0);
    std::fill(m_Textures.begin(), m_Textures.end(), nullptr);
}

void Material::Init(const std::string &name, AssetHandle albedo, AssetHandle metallic, AssetHandle normal,
                    AssetHandle roughness)
{
    Name = name;

    m_Textures[ALBEDO] = AssetManager::GetAsset<Texture2D>(albedo);
    m_Textures[METALLIC] = AssetManager::GetAsset<Texture2D>(metallic);
    m_Textures[NORMAL] = AssetManager::GetAsset<Texture2D>(normal);
    m_Textures[ROUGHNESS] = AssetManager::GetAsset<Texture2D>(roughness);

    m_TextureHandles[ALBEDO] = albedo;
    m_TextureHandles[METALLIC] = metallic;
    m_TextureHandles[NORMAL] = normal;
    m_TextureHandles[ROUGHNESS] = roughness;
}

void Material::Init(const std::string &name, const std::filesystem::path &albedoPath,
                    const std::filesystem::path &aoPath, const std::filesystem::path &metallicPath,
                    const std::filesystem::path &normalPath, const std::filesystem::path &roughnessPath,
                    const std::filesystem::path &alphaMaskPath)
{
    m_Textures[ALBEDO] = AssetManager::GetAsset<Texture2D>(albedoPath);
    m_Textures[AO] = AssetManager::GetAsset<Texture2D>(aoPath);
    m_Textures[METALLIC] = AssetManager::GetAsset<Texture2D>(metallicPath);
    m_Textures[NORMAL] = AssetManager::GetAsset<Texture2D>(normalPath);
    m_Textures[ROUGHNESS] = AssetManager::GetAsset<Texture2D>(roughnessPath);
}

void Material::Init(const std::string_view name, const glm::vec3 &albedo, float ao, const glm::vec3 &normal,
                    float metallic, float roughness, const float alpha)
{
    Name = name;

    m_MaterialParam.Albedo = albedo;
    m_MaterialParam.AO = ao;
    m_MaterialParam.Normal = normal;
    m_MaterialParam.Metallic = metallic;
    m_MaterialParam.Roughness = roughness;
}

unsigned int Material::GetParameterTexture(const ParameterType &type) const noexcept
{
    return m_MaterialTextures[type];
}

void Material::Bind(Shader *shader) const noexcept
{
    if (m_Textures[ParameterType::ALBEDO]) m_Textures[ParameterType::ALBEDO]->Bind(3);
    if (m_Textures[ParameterType::NORMAL]) m_Textures[ParameterType::NORMAL]->Bind(4);
    if (m_Textures[ParameterType::METALLIC]) m_Textures[ParameterType::METALLIC]->Bind(5);
    if (m_Textures[ParameterType::ROUGHNESS]) m_Textures[ParameterType::ROUGHNESS]->Bind(6);

    shader->SetUniform1i("albedoMap", 3);
    shader->SetUniform1i("normalMap", 4);
    shader->SetUniform1i("metallicMap", 5);
    shader->SetUniform1i("roughnessMap", 6);

    shader->SetUniform3f("albedoParam", m_MaterialParam.Albedo);
    shader->SetUniform1f("metallicParam", m_MaterialParam.Metallic);
    shader->SetUniform1f("aoParam", m_MaterialParam.AO);
    shader->SetUniform1f("roughnessParam", m_MaterialParam.Roughness);
}

void Material::Unbind() const noexcept
{
    for (const auto &texture : m_Textures)
    {
        if (texture) texture->Unbind();
    }
}

void Material::SetTexture(ParameterType type, AssetHandle textureHandle)
{
    auto texture = AssetManager::GetAsset<Texture2D>(textureHandle);
    m_Textures[type] = texture;
    m_TextureHandles[type] = textureHandle;
}

void Material::SetMaterialParam(ParameterType type, std::any param)
{
    if (type == ParameterType::ALBEDO)
        m_MaterialParam.Albedo = std::any_cast<glm::vec3>(param);
    else if (type == ParameterType::AO)
        m_MaterialParam.AO = std::any_cast<float>(param);
    else if (type == ParameterType::METALLIC)
        m_MaterialParam.Metallic = std::any_cast<float>(param);
    else if (type == ParameterType::NORMAL)
        m_MaterialParam.Normal = std::any_cast<glm::vec3>(param);
    else if (type == ParameterType::ROUGHNESS)
        m_MaterialParam.Roughness = std::any_cast<float>(param);
}
} // namespace Engine