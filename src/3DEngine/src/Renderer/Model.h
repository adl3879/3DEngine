#pragma once

#include "Mesh.h"

#include <string>
#include <string_view>

struct aiScene;
struct aiNode;
struct aiMesh;

namespace Engine
{
class Model
{
  public:
    Model() = default;
    Model(int entityID, const std::string_view path, const std::string_view name, const bool flipWindingOrder = false,
          const bool loadMaterial = true);
    Model(int entityID, const std::string_view name, const std::vector<Vertex> &vertices,
          std::vector<unsigned int> &indices, const MaterialPtr &material) noexcept;
    Model(int entityID, const std::string_view name, const Mesh &mesh) noexcept;
    virtual ~Model() = default;

    void AttachMesh(const Mesh mesh) noexcept;

    // destroy all opengl handles for sub-meshes
    void Delete();

    auto GetMeshes() const noexcept { return m_Meshes; }

  protected:
    std::vector<Mesh> m_Meshes;

  private:
    bool LoadModel(const std::string_view path, const bool flipWindingOrder, const bool loadMaterial);
    void ProcessNode(aiNode *node, const aiScene *scene, const bool loadMaterial);
    Mesh ProcessMesh(aiMesh *mesh, const aiScene *scene, const bool loadMaterial);

  private:
    std::string m_Name;
    std::string m_Path;
    int m_EntityID;

    size_t m_NumOfMaterials;
};

using ModelPtr = std::shared_ptr<Model>;
} // namespace Engine