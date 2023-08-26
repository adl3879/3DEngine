#pragma once

#include <glm/glm.hpp>
#include <memory>

namespace Engine
{
class ProceduralSky
{
  public:
    ProceduralSky();
    void Draw(glm::mat4 projection, glm::mat4 view);

    glm::vec3 GetSunDirection() const { return SunDirection; };

  public:
    float SurfaceRadius = 6360e3f;
    float AtmosphereRadius = 6380e3f;
    glm::vec3 RayleighScattering = glm::vec3(58e-7f, 135e-7f, 331e-7f);
    glm::vec3 MieScattering = glm::vec3(2e-5f);
    float SunIntensity = 2.0;

    glm::vec3 CenterPoint = glm::vec3(0.f, -SurfaceRadius, 0.f);
    glm::vec3 SunDirection = glm::vec3(0.20000f, 0.95917f, 0.20000f);

    unsigned int VAO;
    unsigned int VBO;
};
using ProceduralSkyRef = std::shared_ptr<ProceduralSky>;
} // namespace Engine