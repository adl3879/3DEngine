#include "Light.h"

namespace Engine
{

Light::Light(LightType type) : m_Type(type) {}

void Light::operator=(const Light &other)
{
    m_Type = other.m_Type;
    m_Color = other.m_Color;
    m_Position = other.m_Position;
    DirectionalLightProps = other.DirectionalLightProps;
    PointLightProps = other.PointLightProps;
    SpotLightProps = other.SpotLightProps;
}

void Light::SetType(int type)
{
    switch (type)
    {
        case 0: m_Type = LightType::Point; break;
        case 1: m_Type = LightType::Spot; break;
        case 2: m_Type = LightType::Directional; break;
        default: m_Type = LightType::None; break;
    }
}

void Light::SetLightUniforms(Shader &shader) const
{
    shader.SetUniform1i("lightType", static_cast<int>(m_Type));
    shader.SetUniform4f("lightColor", m_Color);
    shader.SetUniform3f("lightPos", m_Position);
    switch (m_Type)
    {
        case LightType::Directional:
            shader.SetUniform3f("dLight.direction", DirectionalLightProps.Direction);
            shader.SetUniform1f("dLight.ambient", DirectionalLightProps.Ambient);
            shader.SetUniform1f("dLight.specularLight", DirectionalLightProps.Specular);
            break;
        case LightType::Point:
            shader.SetUniform1f("pLight.ambient", PointLightProps.Ambient);
            shader.SetUniform1f("pLight.specularLight", PointLightProps.Specular);
            shader.SetUniform1f("pLight.constant", PointLightProps.Constant);
            shader.SetUniform1f("pLight.linear", PointLightProps.Linear);
            shader.SetUniform1f("pLight.quadratic", PointLightProps.Quadratic);
            break;
        case LightType::Spot:
            shader.SetUniform1f("sLight.ambient", SpotLightProps.Ambient);
            shader.SetUniform1f("sLight.specularLight", SpotLightProps.Specular);
            shader.SetUniform1f("sLight.cutOff", SpotLightProps.CutOff);
            shader.SetUniform1f("sLight.outerCutOff", SpotLightProps.OuterCutOff);
            break;
        default: break;
    }
}
} // namespace Engine