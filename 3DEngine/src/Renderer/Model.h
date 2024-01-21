#pragma once

#include "StaticMesh.h"

#include <string>
#include <string_view>
#include <filesystem>

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
    virtual ~Model() = default;

    Model(std::filesystem::path path, std::filesystem::path dst = "", const bool flipWindingOrder = false,
          const bool loadMaterial = true);
    Model(const std::string &name, const std::vector<Vertex> &vertices, std::vector<unsigned int> &indices,
          AssetHandle materialHandle) noexcept;
    Model(const StaticMesh &mesh) noexcept;

public:
    void AttachMesh(const StaticMesh mesh) noexcept;

    // destroy all openGL handles for sub-meshes
    void Delete();

    std::vector<StaticMesh> GetMeshes() { return m_Meshes; }
    std::string GetName() { return m_Path.filename().stem().string(); }
    bool HasAnimations() const { return m_HasAnimations; }

    bool LoadModel(const std::filesystem::path &path, const bool flipWindingOrder = false, const bool loadMaterial = true);
private:
    std::vector<StaticMesh> m_Meshes;

private:
    void ProcessNode(aiNode *node, const aiScene *scene, bool loadMaterial);
    StaticMesh ProcessMesh(aiMesh *mesh, const aiScene *scene, const bool loadMaterial);

private:
    std::filesystem::path m_Path, m_Dst;
    size_t m_NumOfMaterials{};
    bool m_HasAnimations = false;
    std::vector<std::string> m_LoadedTextures;
};

using ModelRef = std::shared_ptr<Model>;
} // namespace Engine
