#pragma once

#include <glm/glm.hpp>

namespace Engine
{
class RendererCommand
{
  public:
    static void SetClearColor(const glm::vec4 &color);
    static void Clear();
};

class Renderer3D
{
};
} // namespace Engine