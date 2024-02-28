#pragma once

#include <vector>
#include <memory>
#include <string>

#include "Vertex.h"
#include "VertexArray.h"
#include "Material.h"
#include "Shader.h"

namespace Engine
{
struct SkinnedMesh
{
    SkinnedMesh();
    SkinnedMesh(const std::string &name, const std::vector<SkinnedVertex> &vertices, const std::vector<uint32_t> &indices,
               MaterialRef material = nullptr);

    void Draw(Shader *shader, bool bindMaterials);
    void Clear();

    VertexArray VAO;
    std::string Name;
    std::size_t IndexCount;
    std::size_t VertexCount;

    std::vector<SkinnedVertex> Vertices;
    std::vector<uint32_t> Indices;

	MaterialRef DefaultMaterial;

  private:
    void SetupMesh(const std::vector<SkinnedVertex> &vertices, const std::vector<uint32_t> &indices);
};
using SkinnedMeshRef = std::shared_ptr<SkinnedMesh>;
}