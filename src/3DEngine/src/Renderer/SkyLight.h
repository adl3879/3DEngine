#pragma once

#include <string>
#include <memory>
#include <unordered_map>

#include "Shader.h"
#include "VertexArray.h"
#include "Camera.h"

namespace Engine
{
class SkyLight
{
  public:
    SkyLight() = default;
    virtual ~SkyLight() = default;

    void Init(const std::string &path, const std::size_t resolution);
    void Render(Camera &camera);

  public:
    unsigned int GetIrradianceMap() { return m_IrradianceMap; }
    unsigned int GetPrefilterMap() { return m_PreFilterMap; }
    unsigned int GetBrdfLUT() { return m_BrdfLUT; }
    const std::unordered_map<std::string, ShaderPtr> &GetShaders() { return m_Shaders; }

  private:
    void SetupCube();
    void RenderCube();
    void RenderQuad();

  private:
    unsigned int m_EnvCubemap = 0;
    unsigned int m_IrradianceMap = 0;
    unsigned int m_PreFilterMap = 0;
    unsigned int m_BrdfLUT = 0;

    VertexArray m_CubeVAO{};
    VertexArray m_QuadVAO{};

    std::unordered_map<std::string, ShaderPtr> m_Shaders;
};

using SkyLightPtr = std::shared_ptr<SkyLight>;
} // namespace Engine