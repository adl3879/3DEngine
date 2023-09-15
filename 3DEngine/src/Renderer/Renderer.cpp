#include "Renderer.h"

namespace Engine
{
Shader *Renderer::m_Shader = nullptr;
VertexArray *Renderer::QuadVAO = nullptr;
RenderList Renderer::m_RenderList;

struct RVertex
{
    glm::vec3 Position;
    glm::vec2 UV;
    glm::vec3 Normal;
};

// clang-format off
std::vector<RVertex> QuadVertices
{
    { glm::vec3(-1.0f,  1.0f, 0.0f), glm::vec2(0.0f, 1.0f), glm::vec3(0, 0, -1) },
    { glm::vec3(1.0f,  1.0f, 0.0f),  glm::vec2(1.0f, 1.0f), glm::vec3(0, 0, -1) },
    { glm::vec3(-1.0f, -1.0f, 0.0f), glm::vec2(0, 0),       glm::vec3(0, 0, -1) },
    { glm::vec3(1.0f,  -1.0f, 0.0f), glm::vec2(1.0f, 0.0f), glm::vec3(0, 0, -1) },
    { glm::vec3(-1.0f, -1.0f, 0.0f), glm::vec2(0.0f, 0.0f), glm::vec3(0, 0, -1) },
    { glm::vec3(1.0f,   1.0f, 0.0f), glm::vec2(1.0f, 1.0f), glm::vec3(0, 0, -1) }
};
// clang-format on

void Renderer::Init()
{
    //
}

void Renderer::SubmitMesh(MeshRef mesh, glm::mat4 &transform, const int32_t entityId)
{
    m_RenderList.AddToRenderList(mesh, transform, entityId);
}

void Renderer::Flush(Shader *shader, bool depthOnly) { m_RenderList.Flush(shader, depthOnly); }

void Renderer::BeginDraw(CameraRef camera) {}

void Renderer::EndDraw() {}

void Renderer::DrawQuad(glm::mat4 transform)
{
    QuadVAO->Init();
    QuadVAO->Bind();

    QuadVAO->AttachBuffer(BufferType::ARRAY, QuadVertices.size() * sizeof(RVertex), DrawMode::STATIC,
                          QuadVertices.data());

    // vertex attributes
    const static auto vertexSize = sizeof(RVertex);
    QuadVAO->EnableAttribute(0, 3, vertexSize, reinterpret_cast<void *>(0));
    QuadVAO->EnableAttribute(1, 3, vertexSize, reinterpret_cast<void *>(offsetof(RVertex, UV)));
    QuadVAO->EnableAttribute(2, 3, vertexSize, reinterpret_cast<void *>(offsetof(RVertex, Normal)));

    QuadVAO->Unbind();
}
} // namespace Engine
