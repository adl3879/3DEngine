#pragma once

#include "Camera.h"
#include "Model.h"
#include "Shader.h"
#include "Framebuffer.h"

#include <glm/glm.hpp>

namespace Engine
{
class OutlineSystem
{
  public:
    OutlineSystem() = default;
    ~OutlineSystem() = default;

    void Init();
    void RenderOutline(Camera &camera, Mesh &mesh, const glm::mat4 &modelMatrix, const glm::mat4 &projectionViewMatrix);

  private:
    ShaderPtr m_OutlineShader = nullptr;
    FramebufferPtr m_OutlineFramebuffer = nullptr;
};
} // namespace Engine