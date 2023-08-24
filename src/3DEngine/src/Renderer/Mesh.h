#pragma once

#include <vector>
#include <memory>

#include "Vertex.h"
#include "VertexArray.h"
#include "Material.h"
#include "Asset.h"
#include "Shader.h"

namespace Engine
{
struct Mesh
{
    Mesh(const std::vector<Vertex> &vertices, const std::vector<uint32_t> &indices,
         const MaterialRef &material = nullptr);
    Mesh(const struct VertexSOA &vertices, const std::vector<uint32_t> &indices, const MaterialRef &material = nullptr);

    void Draw(Shader *shader, bool bindMaterials);
    void Clear();

    auto GetTriangleCount() const { return IndexCount / 3; }
    void SetMaterial(AssetHandle handle);

    const std::size_t IndexCount;
    const std::size_t VertexCount;

    std::vector<Vertex> Vertices;
    struct VertexSOA VertexSOA;
    std::vector<uint32_t> Indices;
    VertexArray VAO;
    MaterialRef Material; // default material

    unsigned int VBOs[5];

  private:
    void SetupMesh(const std::vector<Vertex> &vertices, const std::vector<uint32_t> &indices);
    void SetupMesh(const struct VertexSOA &vertices, const std::vector<uint32_t> &indices);
};

using MeshRef = std::shared_ptr<Mesh>;
} // namespace Engine