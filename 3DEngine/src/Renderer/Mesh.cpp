#include "Mesh.h"

#include <glad/glad.h>
#include "RenderCommand.h"

namespace Engine
{
Mesh::Mesh(const std::vector<Vertex> &vertices, const std::vector<uint32_t> &indices, AssetHandle materialHandle)
    : IndexCount(indices.size()), VertexCount(vertices.size()), 
	DefaultMaterialHandle(materialHandle), Vertices(vertices), Indices(indices)
{
    SetupMesh(vertices, indices);
}

void Mesh::Draw(Shader *shader, bool bindMaterials)
{
    VAO.Bind();
    RenderCommand::DrawElements(RendererEnum::TRIANGLES, IndexCount, RendererEnum::UINT, nullptr);

    VAO.Unbind();
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
    VAO.EnableAttribute(1, 2, vertexSize, reinterpret_cast<void *>(offsetof(Vertex, TexCoords)));
    VAO.EnableAttribute(2, 3, vertexSize, reinterpret_cast<void *>(offsetof(Vertex, Normal)));
    VAO.EnableAttribute(3, 3, vertexSize, reinterpret_cast<void *>(offsetof(Vertex, Tangent)));
	VAO.EnableAttribute(4, 3, vertexSize, reinterpret_cast<void *>(offsetof(Vertex, Bitangent)));

    VAO.Unbind();
} 
} // namespace Engine
