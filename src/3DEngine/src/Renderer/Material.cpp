#include "Material.h"

#include "ResourceManager.h"

namespace Engine
{
Material::Material()
{
    std::fill(m_MaterialTextures.begin(), m_MaterialTextures.end(), 0);
    std::fill(m_MaterialColors.begin(), m_MaterialColors.end(), glm::vec3{0.0f});
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

void Material::Init(const std::string_view name, const glm::vec3 &albedo, const glm::vec3 &ao,
                    const glm::vec3 &metallic, const glm::vec3 &normal, const glm::vec3 &roughness, const float alpha)
{
    Name = name;

    m_MaterialColors[ALBEDO] = albedo;
    m_MaterialColors[AO] = ao;
    m_MaterialColors[METALLIC] = metallic;
    m_MaterialColors[NORMAL] = normal;
    m_MaterialColors[ROUGHNESS] = roughness;

    m_Alpha = alpha;
}

unsigned int Material::GetParameterTexture(const ParameterType &type) const noexcept
{
    return m_MaterialTextures[type];
}

glm::vec3 Material::GetParameterColor(const ParameterType &type) const noexcept { return m_MaterialColors[type]; }
} // namespace Engine