#include "Mesh.h"

#include <glad/glad.h>
#include "RenderCommand.h"

namespace Engine
{
Mesh::Mesh(const std::vector<Vertex> &vertices, const std::vector<uint32_t> &indices, const MaterialRef &material)
    : IndexCount(indices.size()), VertexCount(vertices.size()), Material(material), Vertices(vertices), Indices(indices)
{
    SetupMesh(vertices, indices);
}

Mesh::Mesh(const struct VertexSOA &vertices, const std::vector<uint32_t> &indices, const MaterialRef &material)
    : IndexCount(indices.size()), VertexCount(vertices.Positions.size()), Material(material), VertexSOA(vertices),
      Indices(indices)
{
    SetupMesh(vertices, indices);
}

void Mesh::Draw(Shader *shader, bool bindMaterials)
{
    if (bindMaterials) Material->Bind(shader);

    VAO.Bind();
    RenderCommand::DrawElements(RendererEnum::TRIANGLES, IndexCount, RendererEnum::UINT, nullptr);
}

void Mesh::Clear() {}

void Mesh::SetupMesh(const std::vector<Vertex> &vertices, const std::vector<uint32_t> &indices)
{
    VAO.Init();
    VAO.Bind();

    VAO.AttachBuffer(BufferType::ARRAY, vertices.size() * sizeof(Vertex), DrawMode::DYNAMIC, nullptr);
    VAO.AttachBuffer(BufferType::ELEMENT, indices.size() * sizeof(uint32_t), DrawMode::STATIC, indices.data());

    VAO.SetBufferSubData(0, BufferType::ARRAY, 0, sizeof(Vertex) * vertices.size(), vertices.data());

    // vertex attributes
    const static auto vertexSize = sizeof(Vertex);
    VAO.EnableAttribute(0, 3, vertexSize, reinterpret_cast<void *>(0));
    VAO.EnableAttribute(1, 3, vertexSize, reinterpret_cast<void *>(offsetof(Vertex, Normal)));
    VAO.EnableAttribute(2, 3, vertexSize, reinterpret_cast<void *>(offsetof(Vertex, Color)));
    VAO.EnableAttribute(3, 2, vertexSize, reinterpret_cast<void *>(offsetof(Vertex, TexCoords)));

    VAO.Unbind();
}

void Mesh::SetupMesh(const struct VertexSOA &vertices, const std::vector<uint32_t> &indices)
{
    VAO.Init();
    VAO.Bind();

    VAO.AttachBuffer(BufferType::ARRAY, vertices.Positions.size() * sizeof(glm::vec3), DrawMode::DYNAMIC, nullptr);
    VAO.EnableAttribute(0, 3, 0, nullptr);
    VAO.AttachBuffer(BufferType::ARRAY, vertices.Normals.size() * sizeof(glm::vec3), DrawMode::DYNAMIC, nullptr);
    VAO.EnableAttribute(1, 3, 0, nullptr);
    VAO.AttachBuffer(BufferType::ARRAY, vertices.Colors.size() * sizeof(glm::vec3), DrawMode::DYNAMIC, nullptr);
    VAO.EnableAttribute(2, 3, 0, nullptr);
    VAO.AttachBuffer(BufferType::ARRAY, vertices.TexCoords.size() * sizeof(glm::vec2), DrawMode::DYNAMIC, nullptr);
    VAO.EnableAttribute(3, 2, 0, nullptr);

    VAO.AttachBuffer(BufferType::ELEMENT, indices.size() * sizeof(uint32_t), DrawMode::STATIC, indices.data());

    VAO.Unbind();
}
} // namespace Engine