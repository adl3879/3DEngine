#pragma once

#include <vector>

#include "Vertex.h"
#include "VertexArray.h"
#include "Material.h"

namespace Engine
{
struct Mesh
{
    Mesh(const std::vector<Vertex> &vertices, const std::vector<uint32_t> &indices);
    Mesh(const std::vector<Vertex> &vertices, const std::vector<uint32_t> &indices, const MaterialPtr &material);

    void Clear();

    auto GetTriangleCount() const { return IndexCount / 3; }

    const std::size_t IndexCount;
    std::vector<Vertex> Vertices;
    std::vector<uint32_t> Indices;
    VertexArray VAO;
    MaterialPtr Material;

  private:
    void SetupMesh(const std::vector<Vertex> &vertices, const std::vector<uint32_t> &indices);
};
} // namespace Engine