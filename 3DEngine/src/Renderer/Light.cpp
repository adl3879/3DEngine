#include "Light.h"

#include <iostream>
#include "Log.h"

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
        shader.SetUniform3f("gDirectionalLight.Color", glm::vec3(0.0f, 0.0f, 0.0f));
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

glm::mat4 Light::CalcLightSpaceMatrix(Camera &camera, const float nearPlane, const float farPlane)
{	
	const auto proj = glm::perspective(glm::radians(camera.GetFov()), camera.GetAspectRatio(), nearPlane, farPlane);

	auto lightDir = m_DirectionalLightProps != nullptr ? 
		-glm::normalize(m_DirectionalLightProps->Direction) : 
		glm::vec3(-2.0f, 4.0f, -1.0f);
    //lightDir = glm::normalize(glm::vec3(20.0f, 50, 20.0f));
    
	auto frustumCorners = GetFrustumCornersWorldSpace(proj, camera.GetViewMatrix());

	glm::vec3 center = glm::vec3(0, 0, 0);
    for (const auto &v : frustumCorners)
    {
        center += glm::vec3(v);
    }
    center /= frustumCorners.size();

    const auto lightView = glm::lookAt(center + lightDir, center, glm::vec3(0.0f, 1.0f, 0.0f));

	float minX = std::numeric_limits<float>::max();
    float maxX = std::numeric_limits<float>::lowest();
    float minY = std::numeric_limits<float>::max();
    float maxY = std::numeric_limits<float>::lowest();
    float minZ = std::numeric_limits<float>::max();
    float maxZ = std::numeric_limits<float>::lowest();
    for (const auto &v : frustumCorners)
    {
        const auto trf = lightView * v;
        minX = std::min(minX, trf.x);
        maxX = std::max(maxX, trf.x);
        minY = std::min(minY, trf.y);
        maxY = std::max(maxY, trf.y);
        minZ = std::min(minZ, trf.z);
        maxZ = std::max(maxZ, trf.z);
    }

    // Tune this parameter according to the scene
    constexpr float zMult = 10.0f;
    if (minZ < 0)
    {
        minZ *= zMult;
    }
    else
    {
        minZ /= zMult;
    }
    if (maxZ < 0)
    {
        maxZ /= zMult;
    }
    else
    {
        maxZ *= zMult;
    }

    const glm::mat4 lightProjection = glm::ortho(minX, maxX, minY, maxY, minZ, maxZ);

    return lightProjection * lightView;
}

std::vector<glm::vec4> Light::GetFrustumCornersWorldSpace(const glm::mat4 &proj, const glm::mat4 &view)
{
    const auto inv = glm::inverse(proj * view);

    std::vector<glm::vec4> frustumCorners;
    for (unsigned int x = 0; x < 2; ++x)
    {
        for (unsigned int y = 0; y < 2; ++y)
        {
            for (unsigned int z = 0; z < 2; ++z)
            {
                const glm::vec4 pt = inv * glm::vec4(2.0f * x - 1.0f, 2.0f * y - 1.0f, 2.0f * z - 1.0f, 1.0f);
                frustumCorners.push_back(pt / pt.w);
            }
        }
    }

    return frustumCorners;
}

std::vector<glm::mat4> Light::GetLightSpaceMatrices(Camera& camera)
{
    float cameraFarPlane = 500.0f;
    std::vector<float> shadowCascadeLevels{cameraFarPlane / 50.0f, cameraFarPlane / 25.0f, cameraFarPlane / 10.0f,
                                           cameraFarPlane / 2.0f};

    std::vector<glm::mat4> ret;
    for (size_t i = 0; i < shadowCascadeLevels.size() + 1; ++i)
    {
        if (i == 0)
        {
            ret.push_back(CalcLightSpaceMatrix(camera, camera.GetNearClip(), shadowCascadeLevels[i]));
        }
        else if (i < shadowCascadeLevels.size())
        {
            ret.push_back(CalcLightSpaceMatrix(camera, shadowCascadeLevels[i - 1], shadowCascadeLevels[i]));
        }
        else
        {
            ret.push_back(CalcLightSpaceMatrix(camera, shadowCascadeLevels[i - 1], camera.GetFarClip()));
        }
    }
    return ret;
}
} // namespace Engine
