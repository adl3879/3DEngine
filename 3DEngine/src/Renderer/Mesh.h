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
    Mesh() = default;
    Mesh(const std::vector<Vertex> &vertices, const std::vector<uint32_t> &indices, AssetHandle materialHandle = 0);

    void Draw(Shader *shader, bool bindMaterials);
    void Clear();

	void SetHandle(AssetHandle h) { MaterialHandle = h; }

    auto GetTriangleCount() const { return IndexCount / 3; }

    const std::size_t IndexCount;
    const std::size_t VertexCount;

    std::vector<Vertex> Vertices;
    std::vector<uint32_t> Indices;
    VertexArray VAO;

    AssetHandle DefaultMaterialHandle; // default material
	AssetHandle MaterialHandle = 0; // material override

    unsigned int VBOs[5];

  private:
    void SetupMesh(const std::vector<Vertex> &vertices, const std::vector<uint32_t> &indices);
};

using MeshRef = std::shared_ptr<Mesh>;
} // namespace Engine
