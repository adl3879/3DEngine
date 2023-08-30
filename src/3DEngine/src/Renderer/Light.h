#pragma once

#include <glm/glm.hpp>

#include "Camera.h"
#include "Shader.h"

#include <memory>
#include <stdio.h>
#include <map>

#define MAX_POINT_LIGHTS 10
#define MAX_SPOT_LIGHTS 10

#define SNPRINTF snprintf

namespace Engine
{
struct DirectionalLight
{
    glm::vec3 Direction = {1.0f, 0.0f, 0.0f};
    glm::vec3 Color = {0.0f, 0.0f, 0.0f};
    float Intensity = 1.0f;
};

struct PointLight
{
    glm::vec3 Position = {0.0f, 0.0f, 0.0f};
    glm::vec3 Color = {0.0f, 0.0f, 0.0f};
    float Intensity = 1.0f;
};

struct SpotLight
{
    glm::vec3 Position = {0.0f, 0.0f, 0.0f};
    glm::vec3 Direction = {0.0f, 0.0f, 0.0f};
    glm::vec3 Color = {0.0f, 0.0f, 0.0f};
    float Cutoff = 30.0f;
    float OuterCutoff = 35.0f;
    float Intensity = 1.0f;
};

class Light
{
  public:
    Light() = default;
    virtual ~Light() = default;

    void SetLightUniforms(Shader &shader);
    void Reset();

    void SetDirectionalLight(DirectionalLight *directionalLight);
    void SetPointLight(PointLight *pointLight, int index);
    void SetSpotLight(SpotLight *spotlight, int index);

    void RemoveDirectionalLight();
    void RemovePointLight(int index);
    void RemoveSpotLight(int index);

    int GetNumPointLights() { return m_PointLightPropsMap.size(); }
    int GetNumSpotLights() { return m_SpotLightPropsMap.size(); }

  public:
    DirectionalLight *m_DirectionalLightProps = nullptr;
    std::map<int, PointLight *> m_PointLightPropsMap;
    std::map<int, SpotLight *> m_SpotLightPropsMap;
};

using LightRef = std::shared_ptr<Light>;
} // namespace Engine