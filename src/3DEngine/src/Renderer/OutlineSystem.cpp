#include "OutlineSystem.h"

#include <glad/glad.h>
#include "InputManager.h"

#include <memory>

namespace Engine
{
void OutlineSystem::Init()
{
    m_OutlineShader = std::make_shared<Shader>("/Resources/shaders/light.vert", "/Resources/shaders/light.frag");

    WindowState windowState = InputManager::Instance().GetWindowState();
    auto fbSpec = FramebufferSpecification{};
    fbSpec.Attachments = {FramebufferTextureFormat::RGBA8};
    fbSpec.Width = windowState.Width;
    fbSpec.Height = windowState.Height;

    m_OutlineFramebuffer = std::make_shared<Engine::Framebuffer>(fbSpec);
}

void OutlineSystem::RenderOutline(Camera &camera, Mesh &mesh, const glm::mat4 &modelMatrix,
                                  const glm::mat4 &projectionViewMatrix)
{
    // m_OutlineFramebuffer->Bind();

    // Draw the selected mesh with stencil buffer enabled to mark the outline area
    m_OutlineShader->Use();
    m_OutlineShader->SetUniformMatrix4fv("model", modelMatrix);
    m_OutlineShader->SetUniformMatrix4fv("projectionViewMatrix", projectionViewMatrix);
    glDrawElements(GL_TRIANGLES, mesh.IndexCount, GL_UNSIGNED_INT, nullptr);

    // glFinish();
    // m_OutlineFramebuffer->Unbind();
}
} // namespace Engine