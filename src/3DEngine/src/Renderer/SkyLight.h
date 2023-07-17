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

  private:
    void SetupCube();
    void RenderCube();

  private:
    unsigned int m_EnvCubemap = 0;
    unsigned int m_EnvFBO = 0;
    VertexArray m_CubeVAO{};

    std::unordered_map<std::string, ShaderPtr> m_Shaders;
};
} // namespace Engine