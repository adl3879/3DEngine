#pragma once

#include "Mesh.h"

#include <string>
#include <string_view>
#include <filesystem>

#include "Asset.h"

struct aiScene;
struct aiNode;
struct aiMesh;

namespace Engine
{
class Model : public Asset
{
  public:
    Model() = default;
    Model(const std::filesystem::path &path, const bool flipWindingOrder = false, const bool loadMaterial = true);
    Model(const std::vector<Vertex> &vertices, std::vector<unsigned int> &indices,
          const MaterialRef &material) noexcept;
    Model(const Mesh &mesh) noexcept;
    virtual ~Model() = default;

    void AttachMesh(const Mesh mesh) noexcept;

    // destroy all opengl handles for sub-meshes
    void Delete();

    auto GetMeshes() const noexcept { return m_Meshes; }

    virtual AssetType GetType() const override { return AssetType::Mesh; }

  protected:
    std::vector<Mesh> m_Meshes;

  private:
    bool LoadModel(const std::filesystem::path &path, const bool flipWindingOrder, const bool loadMaterial);
    void ProcessNode(aiNode *node, const aiScene *scene, const bool loadMaterial);
    Mesh ProcessMesh(aiMesh *mesh, const aiScene *scene, const bool loadMaterial);

  private:
    std::string m_Path;

    size_t m_NumOfMaterials;
};

using ModelRef = std::shared_ptr<Model>;
} // namespace Engine