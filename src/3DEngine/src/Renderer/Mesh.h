#pragma once

#include <glm/glm.hpp>
#include <vector>

#include "Camera.h"
#include "Texture.h"
#include "VertexArray.h"
#include "Shader.h"
#include "Light.h"

namespace Engine
{
class Mesh
{
  public:
    Mesh() = default;
    Mesh(std::vector<Vertex> &vertices, std::vector<unsigned int> &indices, std::vector<Texture> &textures);
    ~Mesh() = default;

    void Draw(Shader &shader, Camera &camera, const glm::mat4 &modelMatrix = glm::mat4{1.0f});
    void Draw(Shader &shader, Camera &camera, Light &light, const glm::mat4 &modelMatrix = glm::mat4{1.0f});

    std::vector<Vertex> m_Vertices;
    std::vector<unsigned int> m_Indices;
    std::vector<Texture> m_Textures;

    VertexArray m_VAO;
};
} // namespace Engine