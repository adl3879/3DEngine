#include "Material.h"

#include "ResourceManager.h"
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

void Material::Init(const std::string_view name, const std::string_view albedoPath, const std::string_view aoPath,
                    const std::string_view metallicPath, const std::string_view normalPath,
                    const std::string_view roughnessPath, const std::string_view alphaMaskPath)
{
    // TODO: remove if not needed
    Name = name;

    m_MaterialTextures[ALBEDO] = ResourceManager::Instance().LoadTexture(albedoPath);
    m_MaterialTextures[AO] = ResourceManager::Instance().LoadTexture(aoPath);
    m_MaterialTextures[METALLIC] = ResourceManager::Instance().LoadTexture(metallicPath);
    m_MaterialTextures[NORMAL] = ResourceManager::Instance().LoadTexture(normalPath);
    m_MaterialTextures[ROUGHNESS] = ResourceManager::Instance().LoadTexture(roughnessPath);

    m_AlphaMaskTexture = ResourceManager::Instance().LoadTexture(alphaMaskPath);
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

    m_Alpha = alpha;
}

unsigned int Material::GetParameterTexture(const ParameterType &type) const noexcept
{
    return m_MaterialTextures[type];
}

void Material::BindMaterialTextures(uint32_t startIndex) const noexcept
{
    int index = startIndex;
    for (const auto &texture : m_Textures)
    {
        if (texture != nullptr) texture->Bind(index);
        index++;
    }
    if (m_Textures[ParameterType::NORMAL] != nullptr)
    {
        if (!m_UseNormalMap) m_Textures[ParameterType::NORMAL]->Unbind();
    }
}

void Material::UnbindMaterialTextures() const noexcept
{
    for (const auto &texture : m_Textures)
    {
        if (texture != nullptr) texture->Unbind();
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