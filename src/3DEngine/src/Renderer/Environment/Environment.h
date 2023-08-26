#pragma once

#include <glm/glm.hpp>
#include <memory>
#include <string>

#include "SkyLight.h"
#include "ProceduralSky.h"

namespace Engine
{
enum class SkyType
{
    ClearColor = 0,
    ProceduralSky,
    SkyboxHDR,
};

class Environment
{
  public:
    Environment();

    SkyType CurrentSkyType = SkyType::ProceduralSky;

    glm::vec4 AmbientColor = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    SkyLightRef SkyboxHDR;
    ProceduralSkyRef ProceduralSkybox;
};

static std::string SkyTypeToString(SkyType type)
{
    switch (type)
    {
        case SkyType::ClearColor: return "Clear Color";
        case SkyType::ProceduralSky: return "Procedural Sky";
        case SkyType::SkyboxHDR: return "Skybox HDR";
        default: return "Unknown";
    }
}

static SkyType SkyTypeFromString(const std::string &type)
{
    if (type == "Clear Color")
        return SkyType::ClearColor;
    else if (type == "Procedural Sky")
        return SkyType::ProceduralSky;
    else if (type == "Skybox HDR")
        return SkyType::SkyboxHDR;
    else
        return SkyType::ClearColor;
}

using EnvironmentRef = std::shared_ptr<Environment>;
} // namespace Engine