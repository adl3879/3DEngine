#include "Material.h"

#include "ResourceManager.h"
#include "AssetManager.h"
#include "Texture.h"

namespace Engine
{
Material::Material() { std::fill(m_MaterialTextures.begin(), m_MaterialTextures.end(), 0); }

void Material::Init(const std::string &name, AssetHandle albedo, AssetHandle metallic, AssetHandle normal,
                    AssetHandle roughness, AssetHandle alphaMask)
{
    Name = name;

    m_Textures[ALBEDO] = AssetManager::GetAsset<Texture2D>(albedo);
    m_Textures[METALLIC] = AssetManager::GetAsset<Texture2D>(metallic);
    m_Textures[NORMAL] = AssetManager::GetAsset<Texture2D>(normal);
    m_Textures[ROUGHNESS] = AssetManager::GetAsset<Texture2D>(roughness);
}

void Material::Init(const std::string_view name, const std::string_view albedoPath, const std::string_view aoPath,
                    const std::string_view metallicPath, const std::string_view normalPath,
                    const std::string_view roughnessPath, const std::string_view alphaMaskPath)
{
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

    m_Material.Albedo = albedo;
    m_Material.AO = ao;
    m_Material.Normal = normal;
    m_Material.Metallic = metallic;
    m_Material.Roughness = roughness;

    m_Alpha = alpha;
}

unsigned int Material::GetParameterTexture(const ParameterType &type) const noexcept
{
    return m_MaterialTextures[type];
}
} // namespace Engine