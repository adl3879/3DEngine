#pragma once

#include <glm/glm.hpp>

#include "Camera.h"
#include "Shader.h"

#include <memory>

namespace Engine
{
struct BaseLight
{
    glm::vec3 Color = {1.0f, 1.0f, 1.0f};
    float AmbientIntensity = 0.0f;
};

struct DirectionalLight : public BaseLight
{
    glm::vec3 Direction = {0.0f, 0.0f, 0.0f};
    float DiffuseIntensity = 0.0f;
};

class Light
{
  public:
    Light() = default;
    virtual ~Light() = default;

    static void SetLightUniforms(Shader &shader);

    // only one directional light
    static void SetDirectionalLight(const DirectionalLight &directionalLight)
    {
        s_DirectionalLightProps = directionalLight;
    }

  public:
    static DirectionalLight s_DirectionalLightProps;
};
} // namespace Engine