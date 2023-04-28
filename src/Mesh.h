#pragma once

#include <glm/glm.hpp>
#include <vector>

#include "Camera.h"
#include "Texture.h"
#include "VertexArray.h"
#include "Shader.h"

class Mesh
{
  public:
    Mesh() = default;
    Mesh(std::vector<Vertex> &vertices, std::vector<unsigned int> &indices, std::vector<Texture> &textures);
    ~Mesh() = default;

    void LDraw(Shader &shader, Camera &camera);
    void Draw(Shader &shader, Camera &camera, glm::mat4 matrix = glm::mat4{1.0f},
              glm::vec3 translation = glm::vec3{0.0f}, glm::quat rotation = glm::quat{1.0f, 0.0f, 0.0f, 0.0f},
              glm::vec3 scale = glm::vec3{1.0f});

    std::vector<Vertex> m_Vertices;
    std::vector<unsigned int> m_Indices;
    std::vector<Texture> m_Textures;

    VertexArray m_VAO;
};
