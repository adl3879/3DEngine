#pragma once

#include <glm/glm.hpp>

#include <memory>
#include <array>
#include <string_view>

namespace Engine
{
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

    void Init(const std::string_view name, const std::string_view albedoPath, const std::string_view aoPath,
              const std::string_view metallicPath, const std::string_view normalPath,
              const std::string_view roughnessPath, const std::string_view alphaMaskPath);

    void Init(const std::string_view name, const glm::vec3 &albedo, const glm::vec3 &ao, const glm::vec3 &metallic,
              const glm::vec3 &normal, const glm::vec3 &roughness, const float alpha = 1.0f);

    auto operator==(const Material &other) const noexcept { return Name == other.Name; }

    unsigned int GetParameterTexture(const ParameterType &type) const noexcept;
    glm::vec3 GetParameterColor(const ParameterType &type) const noexcept;

    auto GetAlphaValue() const noexcept { return m_Alpha; }
    auto GetAlphaMask() const noexcept { return m_AlphaMaskTexture; }

    std::string_view Name;

  private:
    std::array<unsigned int, 5> m_MaterialTextures;
    std::array<glm::vec3, 5> m_MaterialColors;

    float m_Alpha;
    unsigned int m_AlphaMaskTexture;
};

using MaterialPtr = std::shared_ptr<Material>;
} // namespace Engine