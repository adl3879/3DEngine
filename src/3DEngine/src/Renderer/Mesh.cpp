#include "Mesh.h"

#include <glad/glad.h>

#include "Buffer.h"
#include "InputManager.h"

#include <iostream>

namespace Engine
{
Mesh::Mesh(std::vector<Vertex> &vertices, std::vector<unsigned int> &indices, std::vector<Texture> &textures)
    : m_Vertices(vertices), m_Indices(indices), m_Textures(textures), m_Texture3D{}
{
    VertexBuffer vbo{m_Vertices};
    IndexBuffer ebo{m_Indices};

    VertexBufferLayout layout{
        VertexBufferElement{.Type = ShaderDataType::Float4, .Count = 3, .Normalized = GL_FALSE}, // Position
        VertexBufferElement{.Type = ShaderDataType::Float4, .Count = 3, .Normalized = GL_FALSE}, // Normal
        VertexBufferElement{.Type = ShaderDataType::Float4, .Count = 3, .Normalized = GL_FALSE}, // Color
        VertexBufferElement{.Type = ShaderDataType::Float4, .Count = 2, .Normalized = GL_FALSE}, // Texture
    };
    m_VAO.AddBuffer(vbo, layout);

    m_VAO.Unbind();
    vbo.Unbind();
    ebo.Unbind();
}

Mesh::Mesh(std::vector<float> &vertices, Texture3D &texture)
    : m_Vertices{}, m_Indices{}, m_Textures{}, m_Texture3D{texture}
{
    VertexBuffer vbo(vertices.data(), vertices.size() * sizeof(float));

    VertexBufferLayout layout{
        VertexBufferElement{.Type = ShaderDataType::Float3, .Count = 3, .Normalized = GL_FALSE}, // Position
    };
    m_VAO.AddBuffer(vbo, layout);

    m_VAO.Unbind();
    vbo.Unbind();
}

void Mesh::Draw(Shader &shader, Camera &camera, const glm::mat4 &modelMatrix)
{
    shader.Use();
    m_VAO.Bind();

    // Keep track of how many of each type of textures we have
    unsigned int numDiffuse = 0;
    unsigned int numSpecular = 0;

    for (unsigned int i = 0; i < m_Textures.size(); i++)
    {
        auto texture = m_Textures[i];
        std::string num;
        std::string type = texture.GetType();
        if (type == "diffuse")
        {
            num = std::to_string(numDiffuse++);
        }
        else if (type == "specular")
        {
            num = std::to_string(numSpecular++);
        }
        texture.TextureUnit(shader, (type + num).c_str(), i);
        texture.Bind();
    }

    shader.SetUniformMatrix4fv("model", modelMatrix);
    shader.SetUniformMatrix4fv("projectionViewMatrix", camera.GetProjectionViewMatrix());

    glDrawElements(GL_TRIANGLES, m_Indices.size(), GL_UNSIGNED_INT, 0);

    m_VAO.Unbind();
}

void Mesh::Draw(Shader &shader, Camera &camera, Light &light, const glm::mat4 &modelMatrix)
{
    shader.Use();
    m_VAO.Bind();

    // Keep track of how many of each type of textures we have
    unsigned int numDiffuse = 0;
    unsigned int numSpecular = 0;

    for (unsigned int i = 0; i < m_Textures.size(); i++)
    {
        auto texture = m_Textures[i];
        std::string num;
        std::string type = texture.GetType();
        if (type == "diffuse")
        {
            num = std::to_string(numDiffuse++);
        }
        else if (type == "specular")
        {
            num = std::to_string(numSpecular++);
        }
        texture.TextureUnit(shader, (type + num).c_str(), i);
        texture.Bind();
    }

    shader.SetUniformMatrix4fv("model", modelMatrix);
    shader.SetUniformMatrix4fv("projectionViewMatrix", camera.GetProjectionViewMatrix());
    shader.SetUniform3f("cameraPos", camera.GetPosition());
    light.SetLightUniforms(shader);

    glDrawElements(GL_TRIANGLES, m_Indices.size(), GL_UNSIGNED_INT, 0);

    m_VAO.Unbind();
}

void Mesh::DrawCubeMap(Shader &shader, Camera &camera, const glm::mat4 &modelMatrix)
{
    camera.SetPosition({0.0f, 0.0f, 0.0f});

    glDepthFunc(GL_LEQUAL);

    shader.Use();

    shader.SetUniform1i("cubeMap", 0);
    shader.SetUniformMatrix4fv("model", modelMatrix);
    shader.SetUniformMatrix4fv("projectionViewMatrix", camera.GetProjectionViewMatrix());

    m_VAO.Bind();
    m_Texture3D.Bind();

    glDrawArrays(GL_TRIANGLES, 0, 36);

    m_VAO.Unbind();
    glDepthFunc(GL_LESS);
}
} // namespace Engine