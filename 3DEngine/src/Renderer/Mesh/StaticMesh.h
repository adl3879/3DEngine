#pragma once

#include <vector>
#include <memory>
#include <string>

#include "Vertex.h"
#include "VertexArray.h"
#include "Material.h"
#include "Asset.h"
#include "Shader.h"

namespace Engine
{
struct StaticMesh
{
    StaticMesh();
    StaticMesh(const std::string &name, const std::vector<Vertex> &vertices, const std::vector<uint32_t> &indices,
         AssetHandle materialHandle = 0);
    StaticMesh(const std::string &name, const std::vector<Vertex> &vertices, const std::vector<uint32_t> &indices,
         const std::filesystem::path &materialPath);
	StaticMesh(const std::string &name, const std::vector<Vertex> &vertices, const std::vector<uint32_t> &indices,
         MaterialRef material);

    void Draw(Shader *shader, bool bindMaterials);
    void Clear();

    VertexArray VAO;
    std::string Name;
    std::size_t IndexCount;
    std::size_t VertexCount;

    std::vector<Vertex> Vertices;
    std::vector<uint32_t> Indices;

	MaterialRef DefaultMaterial;

    AssetHandle DefaultMaterialHandle;
    std::filesystem::path DefaultMaterialPath;

  private:
    void SetupMesh(const std::vector<Vertex> &vertices, const std::vector<uint32_t> &indices);
};

using StaticMeshRef = std::shared_ptr<StaticMesh>;
} // namespace Engine
