#pragma once

#include <glm/glm.hpp>

#include "Camera.h"
#include "Shader.h"

#include <memory>

namespace Engine
{
struct DirectionalLight
{
    glm::vec3 Direction = glm::vec3{0.0f};
    float Ambient = 0.1f, Specular = 0.5f;
};

struct PointLight
{
    float Ambient = 0.2f, Specular = 0.5f;
    float Constant = 1.0f, Linear = 0.01f, Quadratic = 0.05f;
};

struct SpotLight
{
    float Ambient = 0.2f, Specular = 0.5f;
    float CutOff = glm::cos(glm::radians(12.5f)), OuterCutOff = glm::cos(glm::radians(15.0f));
};

enum class LightType
{
    Point,
    Spot,
    Directional,
    None,
};

class Light
{
  public:
    Light() = default;
    Light(LightType type);
    virtual ~Light() = default;

    void operator=(const Light &other);

    void SetType(LightType type) { m_Type = type; }
    void SetType(int type);
    void SetLightUniforms(Shader &shader) const;

    void SetColor(const glm::vec4 &color) { m_Color = color; }
    void SetPosition(const glm::vec3 &pos) { m_Position = pos; }

    const glm::vec4 &GetColor() const { return m_Color; }
    const glm::vec3 &GetPosition() const { return m_Position; }

    void SetDirectionalLightProps(const DirectionalLight &props) { DirectionalLightProps = props; }
    void SetPointLightProps(const PointLight &props) { PointLightProps = props; }
    void SetSpotLightProps(const SpotLight &props) { SpotLightProps = props; }

  private:
    LightType m_Type;

    glm::vec4 m_Color{1.0f};
    glm::vec3 m_Position{0.0f};

  public:
    DirectionalLight DirectionalLightProps;
    PointLight PointLightProps;
    SpotLight SpotLightProps;
};
} // namespace Engine