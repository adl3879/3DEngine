#include "StaticMesh.h"

#include <glad/glad.h>
#include "RenderCommand.h"

namespace Engine
{
StaticMesh::StaticMesh() : IndexCount(0), VertexCount(0) {}

StaticMesh::StaticMesh(const std::string &name, const std::vector<Vertex> &vertices, const std::vector<uint32_t> &indices,
           AssetHandle materialHandle)
    : IndexCount(indices.size()), VertexCount(vertices.size()), DefaultMaterialHandle(materialHandle),
      Vertices(vertices), Indices(indices), Name(name)
{
    SetupMesh(vertices, indices);
}

StaticMesh::StaticMesh(const std::string &name, const std::vector<Vertex> &vertices, const std::vector<uint32_t> &indices,
           const std::filesystem::path &materialPath)
    : IndexCount(indices.size()), VertexCount(vertices.size()), DefaultMaterialPath(materialPath),
      Vertices(vertices), Indices(indices), Name(name)
{
    SetupMesh(vertices, indices);
}

StaticMesh::StaticMesh(const std::string& name, const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices,
	MaterialRef material)
	: IndexCount(indices.size()), VertexCount(vertices.size()), DefaultMaterial(material),
	Vertices(vertices), Indices(indices), Name(name)
{
	SetupMesh(vertices, indices);
}

void StaticMesh::Draw(Shader *shader, bool bindMaterials)
{
    VAO.Bind();
    RenderCommand::DrawElements(RendererEnum::TRIANGLES, IndexCount, RendererEnum::UINT, nullptr);
    VAO.Unbind();
}

void StaticMesh::Clear() { VAO.Delete(); }

void StaticMesh::SetupMesh(const std::vector<Vertex> &vertices, const std::vector<uint32_t> &indices)
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
