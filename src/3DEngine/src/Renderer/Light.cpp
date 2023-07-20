#include "Light.h"

#include <iostream>

namespace Engine
{
DirectionalLight *Light::s_DirectionalLightProps = nullptr;
std::map<int, PointLight> Light::s_PointLightPropsMap = {};
std::map<int, SpotLight> Light::s_SpotLightPropsMap = {};

void Light::SetLightUniforms(Shader &shader)
{
    // directional
    if (s_DirectionalLightProps != nullptr)
    {
        shader.SetUniform3f("gDirectionalLight.Base.Color", s_DirectionalLightProps->Color);
        shader.SetUniform1f("gDirectionalLight.Base.AmbientIntensity", s_DirectionalLightProps->AmbientIntensity);
        shader.SetUniform1f("gDirectionalLight.Base.DiffuseIntensity", s_DirectionalLightProps->DiffuseIntensity);
        shader.SetUniform3f("gDirectionalLight.Direction", s_DirectionalLightProps->Direction);
    }
    else
    {
        shader.SetUniform3f("gDirectionalLight.Base.Color", glm::vec3(0.0f, 0.0f, 0.0f));
        shader.SetUniform1f("gDirectionalLight.Base.AmbientIntensity", 0.0f);
        shader.SetUniform1f("gDirectionalLight.Base.DiffuseIntensity", 0.0f);
        shader.SetUniform3f("gDirectionalLight.Direction", glm::vec3(0.0f, 0.0f, 0.0f));
    }

    // point
    {
        auto it = s_PointLightPropsMap.begin();
        for (unsigned int i = 0; i < s_PointLightPropsMap.size(); i++)
        {
            char buffer[100];
            SNPRINTF(buffer, sizeof(buffer), "gPointLights[%d].Color", i);
            shader.SetUniform3f(buffer, it->second.Color);
            // SNPRINTF(buffer, sizeof(buffer), "gPointLights[%d].Base.AmbientIntensity", i);
            // shader.SetUniform1f(buffer, it->second.AmbientIntensity);
            // SNPRINTF(buffer, sizeof(buffer), "gPointLights[%d].Base.DiffuseIntensity", i);
            // shader.SetUniform1f(buffer, it->second.DiffuseIntensity);
            SNPRINTF(buffer, sizeof(buffer), "gPointLights[%d].Position", i);
            shader.SetUniform3f(buffer, it->second.Position);
            // SNPRINTF(buffer, sizeof(buffer), "gPointLights[%d].Atten.Constant", i);
            // shader.SetUniform1f(buffer, it->second.Attenuation.Constant);
            // SNPRINTF(buffer, sizeof(buffer), "gPointLights[%d].Atten.Linear", i);
            // shader.SetUniform1f(buffer, it->second.Attenuation.Linear);
            // SNPRINTF(buffer, sizeof(buffer), "gPointLights[%d].Atten.Exp", i);
            // shader.SetUniform1f(buffer, it->second.Attenuation.Exp);

            it++;
        }
        shader.SetUniform1i("numPointLights", s_PointLightPropsMap.size());
    }

    // spot
    {
        auto it = s_SpotLightPropsMap.begin();
        for (unsigned int i = 0; i < s_SpotLightPropsMap.size(); i++)
        {
            char buffer[100];
            SNPRINTF(buffer, sizeof(buffer), "gSpotLights[%d].Base.Base.Color", i);
            shader.SetUniform3f(buffer, it->second.Color);
            SNPRINTF(buffer, sizeof(buffer), "gSpotLights[%d].Base.Base.AmbientIntensity", i);
            shader.SetUniform1f(buffer, it->second.AmbientIntensity);
            SNPRINTF(buffer, sizeof(buffer), "gSpotLights[%d].Base.Base.DiffuseIntensity", i);
            shader.SetUniform1f(buffer, it->second.DiffuseIntensity);
            SNPRINTF(buffer, sizeof(buffer), "gSpotLights[%d].Base.Position", i);
            shader.SetUniform3f(buffer, it->second.Position);
            SNPRINTF(buffer, sizeof(buffer), "gSpotLights[%d].Base.Atten.Constant", i);
            shader.SetUniform1f(buffer, it->second.Attenuation.Constant);
            SNPRINTF(buffer, sizeof(buffer), "gSpotLights[%d].Base.Atten.Linear", i);
            shader.SetUniform1f(buffer, it->second.Attenuation.Linear);
            SNPRINTF(buffer, sizeof(buffer), "gSpotLights[%d].Base.Atten.Exp", i);
            shader.SetUniform1f(buffer, it->second.Attenuation.Exp);
            SNPRINTF(buffer, sizeof(buffer), "gSpotLights[%d].Direction", i);
            shader.SetUniform3f(buffer, it->second.Direction);
            SNPRINTF(buffer, sizeof(buffer), "gSpotLights[%d].Cutoff", i);
            shader.SetUniform1f(buffer, glm::cos(glm::radians(it->second.Cutoff)));
            SNPRINTF(buffer, sizeof(buffer), "gSpotLights[%d].OuterCutoff", i);
            shader.SetUniform1f(buffer, glm::cos(glm::radians(it->second.OuterCutoff)));

            it++;
        }
        shader.SetUniform1i("gNumSpotLights", s_SpotLightPropsMap.size());
    }
}

void Light::Reset()
{
    s_DirectionalLightProps = nullptr;
    s_PointLightPropsMap.clear();
    s_SpotLightPropsMap.clear();
}

void Light::SetDirectionalLight(DirectionalLight *directionalLight) { s_DirectionalLightProps = directionalLight; }

void Light::SetPointLight(const PointLight &pointLight, int index) { s_PointLightPropsMap[index] = pointLight; }

void Light::SetSpotLight(const SpotLight &spotlight, int index) { s_SpotLightPropsMap[index] = spotlight; }

void Light::RemoveDirectionalLight() { s_DirectionalLightProps = nullptr; }

void Light::RemovePointLight(int index)
{
    auto it = s_PointLightPropsMap.find(index);
    s_PointLightPropsMap.erase(it);
}

void Light::RemoveSpotLight(int index)
{
    auto it = s_SpotLightPropsMap.find(index);
    s_SpotLightPropsMap.erase(it);
}
} // namespace Engine