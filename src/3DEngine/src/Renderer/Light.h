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
    glm::vec3 Color = {1.0f, 1.0f, 1.0f};
    float Intensity = 1.0f;
};

struct PointLight
{
    glm::vec3 Position = {0.0f, 0.0f, 0.0f};
    glm::vec3 Color = {1.0f, 1.0f, 1.0f};
    float Intensity = 1.0f;
};

struct SpotLight
{
    glm::vec3 Position = {0.0f, 0.0f, 0.0f};
    glm::vec3 Direction = {0.0f, 0.0f, 0.0f};
    glm::vec3 Color = {1.0f, 1.0f, 1.0f};
    float Cutoff = 30.0f;
    float OuterCutoff = 35.0f;
    float Intensity = 1.0f;
};

class Light
{
  public:
    Light() = default;
    virtual ~Light() = default;

    static void SetLightUniforms(Shader &shader);
    static void Reset();

    static void SetDirectionalLight(DirectionalLight *directionalLight);
    static void SetPointLight(const PointLight &pointLight, int index);
    static void SetSpotLight(const SpotLight &spotlight, int index);

    static void RemoveDirectionalLight();
    static void RemovePointLight(int index);
    static void RemoveSpotLight(int index);

    static int GetNumPointLights() { return s_PointLightPropsMap.size(); }
    static int GetNumSpotLights() { return s_SpotLightPropsMap.size(); }

  public:
    static DirectionalLight *s_DirectionalLightProps;
    static std::map<int, PointLight> s_PointLightPropsMap;
    static std::map<int, SpotLight> s_SpotLightPropsMap;
};
} // namespace Engine