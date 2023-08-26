#pragma once

#include <string>
#include <memory>
#include <unordered_map>
#include <glm/glm.hpp>

#include "Shader.h"
#include "VertexArray.h"
#include "Camera.h"
#include "Asset.h"

namespace Engine
{
class SkyLight
{
  public:
    SkyLight() = default;
    virtual ~SkyLight() = default;

    void Init(AssetHandle handle, const std::size_t resolution);
    void Destroy();
    void Render(const glm::mat4 &projection, const glm::mat4 &view);

  public:
    unsigned int GetIrradianceMap() { return m_IrradianceMap; }
    unsigned int GetPrefilterMap() { return m_PreFilterMap; }
    unsigned int GetBrdfLUT() { return m_BrdfLUT; }
    const std::unordered_map<std::string, ShaderPtr> &GetShaders() { return m_Shaders; }

    AssetHandle GetHandle() const { return m_EnvironmentHandle; }

    void BindMaps(int slot = 0) const;
    void UnBindMaps() const;

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
    AssetHandle m_EnvironmentHandle;
};

using SkyLightRef = std::shared_ptr<SkyLight>;
} // namespace Engine