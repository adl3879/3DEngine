#pragma once

#include "Mesh.h"

#include <string>
#include <string_view>
#include <filesystem>

#include "Asset.h"

struct aiScene;
struct aiNode;
struct aiMesh;
struct aiString;

namespace Engine
{
class Model
{
public:
    Model() = default;
    Model(std::filesystem::path path, std::filesystem::path dst, const bool flipWindingOrder = false,
          const bool loadMaterial = true);
    Model(const std::string &name, const std::vector<Vertex> &vertices, std::vector<unsigned int> &indices,
          AssetHandle materialHandle) noexcept;
    Model(const Mesh &mesh) noexcept;
    virtual ~Model() = default;

public:
    void AttachMesh(const Mesh mesh) noexcept;

    // destroy all openGL handles for sub-meshes
    void Delete();

    std::vector<Mesh> GetMeshes() { return m_Meshes; }
    std::string GetName() { return m_Path.filename().stem().string(); }

private:
    std::vector<Mesh> m_Meshes;

private:
    bool LoadModel(const std::filesystem::path &path, const bool flipWindingOrder, const bool loadMaterial);
    void ProcessNode(aiNode *node, const aiScene *scene, bool loadMaterial);
    Mesh ProcessMesh(aiMesh *mesh, const aiScene *scene, const bool loadMaterial);

private:
    std::filesystem::path m_Path, m_Dst;
    size_t m_NumOfMaterials{};
};

using ModelRef = std::shared_ptr<Model>;
} // namespace Engine
