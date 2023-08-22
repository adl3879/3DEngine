#pragma once

#include <glm/glm.hpp>

#include "Renderer.h"
#include "Scene.h"
#include "Framebuffer.h"

namespace Engine
{
class SceneRenderer
{
  public:
    void Init();
    void Cleanup();

    void BeginRenderScene(const glm::mat4 &projection, const glm::mat4 &view, const glm::vec3 &cameraPosition);
    void RenderScene(Scene &scene, Framebuffer &framebuffer);
};
} // namespace Engine