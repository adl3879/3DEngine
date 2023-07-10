#include "Mesh.h"

namespace Engine
{
Mesh::Mesh(const std::vector<Vertex> &vertices, const std::vector<uint32_t> &indices)
    : IndexCount(indices.size()), Vertices(vertices), Indices(indices)
{
    SetupMesh(vertices, indices);
}

Mesh::Mesh(const std::vector<Vertex> &vertices, const std::vector<uint32_t> &indices, const MaterialPtr &material)
    : IndexCount(indices.size()), Material(material), Vertices(vertices), Indices(indices)
{
    SetupMesh(vertices, indices);
}

void Mesh::Clear() {}

void Mesh::SetupMesh(const std::vector<Vertex> &vertices, const std::vector<uint32_t> &indices)
{
    VAO.Init();
    VAO.Bind();

    VAO.AttachBuffer(BufferType::ARRAY, vertices.size() * sizeof(Vertex), DrawMode::DYNAMIC, vertices.data());
    VAO.AttachBuffer(BufferType::ELEMENT, indices.size() * sizeof(uint32_t), DrawMode::STATIC, indices.data());

    // vertex attributes
    const static auto vertexSize = sizeof(Vertex);
    VAO.EnableAttribute(0, 3, vertexSize, reinterpret_cast<void *>(0));
    VAO.EnableAttribute(1, 3, vertexSize, reinterpret_cast<void *>(offsetof(Vertex, Normal)));
    VAO.EnableAttribute(2, 3, vertexSize, reinterpret_cast<void *>(offsetof(Vertex, Color)));
    VAO.EnableAttribute(3, 2, vertexSize, reinterpret_cast<void *>(offsetof(Vertex, TexCoords)));
    VAO.EnableAttribute(4, 1, vertexSize, reinterpret_cast<void *>(offsetof(Vertex, EditorID)));

    VAO.Unbind();
}
} // namespace Engine