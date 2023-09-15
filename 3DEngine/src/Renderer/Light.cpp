#include "Light.h"

#include <iostream>

namespace Engine
{
void Light::SetLightUniforms(Shader &shader)
{
    // directional
    if (m_DirectionalLightProps)
    {
        shader.SetUniform3f("gDirectionalLight.Color",
                            m_DirectionalLightProps->Color * m_DirectionalLightProps->Intensity);
        shader.SetUniform3f("gDirectionalLight.Direction", m_DirectionalLightProps->Direction);
    }
    else
    {
        shader.SetUniform3f("gDirectionalLight.Base.Color", glm::vec3(0.0f, 0.0f, 0.0f));
        shader.SetUniform3f("gDirectionalLight.Direction", glm::vec3(0.0f, 0.0f, 0.0f));
    }

    // point
    {
        auto it = m_PointLightPropsMap.begin();
        for (unsigned int i = 0; i < m_PointLightPropsMap.size(); i++)
        {
            if (it->second)
            {
                char buffer[100];
                SNPRINTF(buffer, sizeof(buffer), "gPointLights[%d].Color", i);
                shader.SetUniform3f(buffer, it->second->Color * it->second->Intensity);
                SNPRINTF(buffer, sizeof(buffer), "gPointLights[%d].Position", i);
                shader.SetUniform3f(buffer, it->second->Position);
            }
            else
            {
                char buffer[100];
                SNPRINTF(buffer, sizeof(buffer), "gPointLights[%d].Color", i);
                shader.SetUniform3f(buffer, glm::vec3(0.0f, 0.0f, 0.0f));
                SNPRINTF(buffer, sizeof(buffer), "gPointLights[%d].Position", i);
                shader.SetUniform3f(buffer, glm::vec3(0.0f, 0.0f, 0.0f));
            }
            it++;
        }
        shader.SetUniform1i("gNumOfPointLights", m_PointLightPropsMap.size());
    }

    // spot
    {
        auto it = m_SpotLightPropsMap.begin();
        for (unsigned int i = 0; i < m_SpotLightPropsMap.size(); i++)
        {
            if (it->second)
            {
                char buffer[100];
                SNPRINTF(buffer, sizeof(buffer), "gSpotLights[%d].Color", i);
                shader.SetUniform3f(buffer, it->second->Color * it->second->Intensity);
                SNPRINTF(buffer, sizeof(buffer), "gSpotLights[%d].Direction", i);
                shader.SetUniform3f(buffer, it->second->Direction);
                SNPRINTF(buffer, sizeof(buffer), "gSpotLights[%d].Position", i);
                shader.SetUniform3f(buffer, it->second->Position);
                SNPRINTF(buffer, sizeof(buffer), "gSpotLights[%d].Cutoff", i);
                shader.SetUniform1f(buffer, glm::cos(glm::radians(it->second->Cutoff)));
                SNPRINTF(buffer, sizeof(buffer), "gSpotLights[%d].OuterCutoff", i);
                shader.SetUniform1f(buffer, glm::cos(glm::radians(it->second->OuterCutoff)));
            }
            else
            {
                char buffer[100];
                SNPRINTF(buffer, sizeof(buffer), "gSpotLights[%d].Color", i);
                shader.SetUniform3f(buffer, glm::vec3(0.0f, 0.0f, 0.0f));
                SNPRINTF(buffer, sizeof(buffer), "gSpotLights[%d].Direction", i);
                shader.SetUniform3f(buffer, glm::vec3(0.0f, 0.0f, 0.0f));
                SNPRINTF(buffer, sizeof(buffer), "gSpotLights[%d].Position", i);
                shader.SetUniform3f(buffer, glm::vec3(0.0f, 0.0f, 0.0f));
                SNPRINTF(buffer, sizeof(buffer), "gSpotLights[%d].Cutoff", i);
                shader.SetUniform1f(buffer, 0.0f);
                SNPRINTF(buffer, sizeof(buffer), "gSpotLights[%d].OuterCutoff", i);
                shader.SetUniform1f(buffer, 0.0f);
            }
            it++;
        }
        shader.SetUniform1i("gNumOfSpotLights", m_SpotLightPropsMap.size());
    }
}

void Light::Reset()
{
    m_DirectionalLightProps = nullptr;
    m_PointLightPropsMap.clear();
    m_SpotLightPropsMap.clear();
}

void Light::SetDirectionalLight(DirectionalLight *directionalLight) { m_DirectionalLightProps = directionalLight; }

void Light::SetPointLight(PointLight *pointLight, int index) { m_PointLightPropsMap[index] = pointLight; }

void Light::SetSpotLight(SpotLight *spotlight, int index) { m_SpotLightPropsMap[index] = spotlight; }

void Light::RemoveDirectionalLight() { m_DirectionalLightProps = nullptr; }

void Light::RemovePointLight(int index) { m_PointLightPropsMap[index] = nullptr; }

void Light::RemoveSpotLight(int index) { m_SpotLightPropsMap[index] = nullptr; }
} // namespace Engine
