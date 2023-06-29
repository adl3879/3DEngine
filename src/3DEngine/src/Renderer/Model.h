#pragma once

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <glm/glm.hpp>
#include <vector>

#include "Shader.h"
#include "Camera.h"
#include "Texture.h"
#include "Buffer.h"
#include "Mesh.h"
#include "Light.h"
#include "Material.h"

namespace Engine
{
class Model
{
  public:
    Model() = default;
    Model(const char *file);

    void Draw(Shader &shader, Camera &camera, const glm::mat4 &transform = glm::mat4(1.0f));

  private:
    std::vector<unsigned int> GetIndices(const aiMesh *mesh);
    std::vector<Vertex> AssembleVertices(const aiMesh *mesh);
    std::vector<Texture> GetTextures();
    Material GetMaterial();

    void LoadMesh();

  private:
    const char *m_File;
    std::vector<unsigned char> m_Data;
    const aiScene *m_Scene;

    std::vector<Mesh> m_Meshes;
    bool m_HasTexture = false;
    std::vector<Texture> m_TexturesLoaded;
    Material m_Material;

  private:
    glm::mat4 m_ModelMatrix{};
};
} // namespace Engine