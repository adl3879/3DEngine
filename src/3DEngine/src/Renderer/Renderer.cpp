#include "Renderer.h"

namespace Engine
{
Shader *Renderer::m_Shader = nullptr;

void Renderer::Init()
{
    //
}

void Renderer::SubmitMesh(MeshRef mesh, const glm::mat4 &transform, const int32_t entityId)
{
    m_RenderList.AddToRenderList(mesh, transform, entityId);
}

void Renderer::Flush(Shader *shader, bool depthOnly) { m_RenderList.Flush(shader, depthOnly); }

void Renderer::BeginDraw(CameraRef camera)
{
    m_Shader->Bind();
    m_Shader->SetUniformMatrix4fv("u_Projection", camera->GetProjectionMatrix());
    m_Shader->SetUniformMatrix4fv("u_View", camera->GetViewMatrix());
    m_Shader->SetUniform3f("u_CameraPosition", camera->GetPosition());
}

void Renderer::EndDraw() {}
} // namespace Engine
