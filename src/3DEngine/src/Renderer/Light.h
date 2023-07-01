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
struct BaseLight
{
    glm::vec3 Color = {1.0f, 1.0f, 1.0f};
    float AmbientIntensity = 0.0f;
    float DiffuseIntensity = 0.1f;
};

struct DirectionalLight : public BaseLight
{
    glm::vec3 Direction = {1.0f, 0.0f, 0.0f};
};

struct LightAttenuation
{
    float Constant = 1.0f;
    float Linear = 0.002f;
    float Exp = 0.003f;
};

struct PointLight : public BaseLight
{
    glm::vec3 Position = {0.0f, 0.0f, 0.0f};
    LightAttenuation Attenuation;
};

struct SpotLight : public PointLight
{
    glm::vec3 Direction = {0.0f, 0.0f, 0.0f};
    float Cutoff = 30.0f;
    float OuterCutoff = 35.0f;
};

class Light
{
  public:
    Light() = default;
    virtual ~Light() = default;

    static void SetLightUniforms(Shader &shader);

    static void SetDirectionalLight(DirectionalLight *directionalLight);
    static void SetPointLight(const PointLight &pointLight, int index);
    static void SetSpotLight(const SpotLight &spotlight, int index);

    static void RemoveDirectionalLight();
    static void RemovePointLight(int index);
    static void RemoveSpotLight(int index);

  public:
    static DirectionalLight *s_DirectionalLightProps;
    static std::map<int, PointLight> s_PointLightPropsMap;
    static std::map<int, SpotLight> s_SpotLightPropsMap;
};
} // namespace Engine