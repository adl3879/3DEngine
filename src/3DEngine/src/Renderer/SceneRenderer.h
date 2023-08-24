#pragma once

#include <glm/glm.hpp>

#include "Renderer.h"
#include "Scene.h"
#include "Framebuffer.h"

#include <memory>

namespace Engine
{
class SceneRenderer
{
  public:
    void Init();
    void Cleanup();

    void BeginRenderScene(const glm::mat4 &projection, const glm::mat4 &view, const glm::vec3 &cameraPosition);
    void RenderScene(Scene &scene, Framebuffer &framebuffer);

  private:
    glm::mat4 m_Projection, m_View;
    glm::vec3 m_CameraPosition;

    std::unique_ptr<Framebuffer> m_GBuffer;
    std::unique_ptr<Framebuffer> m_ShadingBuffer;

  private:
    void GBufferPass(Scene &scene);
    void ShadingPass(Scene &scene);
};
} // namespace Engine