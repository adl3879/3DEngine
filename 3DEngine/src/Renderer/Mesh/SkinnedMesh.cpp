#include "SkinnedMesh.h"

#include <glad/glad.h>
#include "RenderCommand.h"

namespace Engine
{
SkinnedMesh::SkinnedMesh() : IndexCount(0), VertexCount(0) {}


SkinnedMesh::SkinnedMesh(const std::string &name, const std::vector<SkinnedVertex> &vertices,
                       const std::vector<uint32_t> &indices, MaterialRef material)
    : IndexCount(indices.size()), VertexCount(vertices.size()), DefaultMaterial(material), Vertices(vertices),
      Indices(indices), Name(name)
{
    SetupMesh(vertices, indices);
}

void SkinnedMesh::Draw(Shader *shader, bool bindMaterials)
{
    VAO.Bind();
    RenderCommand::DrawElements(RendererEnum::TRIANGLES, IndexCount, RendererEnum::UINT, nullptr);
    VAO.Unbind();
}

void SkinnedMesh::Clear() { VAO.Delete(); }

void SkinnedMesh::SetupMesh(const std::vector<SkinnedVertex> &vertices, const std::vector<uint32_t> &indices)
{
    VAO.Init();
    VAO.Bind();

    VAO.AttachBuffer(BufferType::ARRAY, vertices.size() * sizeof(SkinnedVertex), DrawMode::DYNAMIC, nullptr);
    VAO.AttachBuffer(BufferType::ELEMENT, indices.size() * sizeof(uint32_t), DrawMode::STATIC, indices.data());

    VAO.SetBufferSubData(0, BufferType::ARRAY, 0, sizeof(SkinnedVertex) * vertices.size(), vertices.data());

    // vertex attributes
    const static auto vertexSize = sizeof(SkinnedVertex);
    VAO.EnableAttribute(0, 3, vertexSize, reinterpret_cast<void *>(0));
    VAO.EnableAttribute(1, 2, vertexSize, reinterpret_cast<void *>(offsetof(SkinnedVertex, TexCoords)));
    VAO.EnableAttribute(2, 3, vertexSize, reinterpret_cast<void *>(offsetof(SkinnedVertex, Normal)));
    VAO.EnableAttribute(3, 3, vertexSize, reinterpret_cast<void *>(offsetof(SkinnedVertex, Tangent)));
    VAO.EnableAttribute(4, 3, vertexSize, reinterpret_cast<void *>(offsetof(SkinnedVertex, Bitangent)));
    
	VAO.EnableAttributeI(5, 4, vertexSize, reinterpret_cast<void *>(offsetof(SkinnedVertex, BoneIDs)));
    VAO.EnableAttribute(6, 4, vertexSize, reinterpret_cast<void *>(offsetof(SkinnedVertex, Weights)));

    VAO.Unbind();
}
} // namespace Engine
