#include "Renderer.h"

#include <glad/glad.h>

namespace Engine
{
void RendererCommand::SetClearColor(const glm::vec4 &color) { glClearColor(color.x, color.y, color.z, color.a); }

void RendererCommand::Clear() { glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); }
} // namespace Engine