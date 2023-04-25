#include "Mesh.h"
#include "Buffer.h"

#include <iostream>

Mesh::Mesh(std::vector<Vertex> &vertices, std::vector<unsigned int> &indices, std::vector<Texture> &textures)
    : m_Vertices(vertices), m_Indices(indices), m_Textures(textures)
{
    VertexBuffer vbo{m_Vertices};
    IndexBuffer ebo{m_Indices};

    VertexBufferLayout layout{
        VertexBufferElement{.Type = GL_FLOAT, .Count = 3, .Normalized = GL_FALSE}, // Position
        VertexBufferElement{.Type = GL_FLOAT, .Count = 3, .Normalized = GL_FALSE}, // Normal
        VertexBufferElement{.Type = GL_FLOAT, .Count = 3, .Normalized = GL_FALSE}, // Color
        VertexBufferElement{.Type = GL_FLOAT, .Count = 2, .Normalized = GL_FALSE}, // Texture
    };
    m_VAO.AddBuffer(vbo, layout);

    m_VAO.Unbind();
    vbo.Unbind();
    ebo.Unbind();
}

void Mesh::Draw(Shader &shader, Camera &camera)
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

    glm::mat4 projection{1.0f};
    projection = glm::perspective(45.0f, 800.0f / 1000.0f, 0.1f, 100.0f);

    shader.SetUniformMatrix4fv("projection", projection);

    shader.SetUniformMatrix4fv("view", camera.GetViewMatrix());

    glDrawElements(GL_TRIANGLES, m_Indices.size(), GL_UNSIGNED_INT, 0);

    m_VAO.Unbind();
}
