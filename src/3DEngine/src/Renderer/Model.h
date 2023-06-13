#pragma once

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <vector>

#include "Shader.h"
#include "Camera.h"
#include "Texture.h"
#include "Buffer.h"
#include "Mesh.h"
#include "Light.h"

namespace Engine
{
class Model
{
  public:
    Model() = default;
    Model(const char *file);

    void Draw(Shader &shader, Camera &camera);
    void Draw(Shader &shader, Camera &camera, Light &light);

    void SetPosition(const glm::vec3 &position);
    void SetRotation(const glm::vec3 &rotation);
    void SetScale(const glm::vec3 &scale);

    const glm::vec3 &GetPosition() const { return m_Position; }
    const glm::vec3 &GetRotation() const { return m_Rotation; }
    const glm::vec3 &GetScale() const { return m_Scale; }

  private:
    void RecalculateModelMatrix();

  private:
    std::vector<unsigned int> GetIndices(const aiMesh *mesh);
    std::vector<Vertex> AssembleVertices(const aiMesh *mesh);
    std::vector<Texture> GetTextures();

    void LoadMesh();

  private:
    const char *m_File;
    std::vector<unsigned char> m_Data;
    const aiScene *m_Scene;

    std::vector<Mesh> m_Meshes;
    bool m_HasTexture = false;
    std::vector<Texture> m_TexturesLoaded;

  private:
    glm::vec3 m_Position = glm::vec3(0.0f, 2.093f, 0.5f);
    glm::vec3 m_Rotation = glm::vec3(0.0f);
    glm::vec3 m_Scale = glm::vec3(1.0f);
    glm::mat4 m_ModelMatrix{};
};
} // namespace Engine