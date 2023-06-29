#include "Light.h"

namespace Engine
{
DirectionalLight Light::s_DirectionalLightProps = DirectionalLight{};

void Light::SetLightUniforms(Shader &shader)
{
    shader.SetUniform3f("gDirectionalLight.Color", s_DirectionalLightProps.Color);
    shader.SetUniform1f("gDirectionalLight.AmbientIntensity", s_DirectionalLightProps.AmbientIntensity);
    shader.SetUniform1f("gDirectionalLight.DiffuseIntensity", s_DirectionalLightProps.DiffuseIntensity);
    shader.SetUniform3f("gDirectionalLight.Direction", s_DirectionalLightProps.Direction);
}
} // namespace Engine