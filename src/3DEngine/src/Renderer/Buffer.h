#pragma once

#include "Shader.h"
#include <vector>

namespace Engine
{
struct VertexBufferElement
{
  public:
    unsigned int Type;
    unsigned int Count;
    size_t Offset;
    unsigned char Normalized;

    static unsigned int GetSizeOfType(unsigned int type)
    {
        switch (type)
        {
        case GL_FLOAT:
            return 4;
        case GL_UNSIGNED_INT:
            return 4;
        case GL_UNSIGNED_BYTE:
            return 1;
        default:
            return 0;
        }
    }
};

class VertexBufferLayout
{
  public:
    VertexBufferLayout() = default;
    ~VertexBufferLayout() = default;

    VertexBufferLayout(std::initializer_list<VertexBufferElement> elements) : m_Elements(elements)
    {
        CalculateOffsetsAndStride();
    }

    inline const std::vector<VertexBufferElement> &GetElements() const { return m_Elements; }
    inline unsigned int GetStride() const { return m_Stride; }

  private:
    void CalculateOffsetsAndStride()
    {
        size_t offset = 0;
        m_Stride = 0;
        for (auto &element : m_Elements)
        {
            element.Offset = offset;
            offset += element.Count;
            m_Stride += element.Count;
        }
    }

  private:
    std::vector<VertexBufferElement> m_Elements;
    unsigned int m_Stride;
};

struct Vertex
{
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec3 Color;
    glm::vec2 Texture;
};

class VertexBuffer
{
  public:
    VertexBuffer() = default;
    virtual ~VertexBuffer();

    VertexBuffer(float *vertices, uint32_t size);
    VertexBuffer(std::vector<Vertex> &vertices);

    void Bind() const;
    void Unbind() const;

  private:
    unsigned int m_VertexBuffer;
};

class IndexBuffer
{
  public:
    IndexBuffer() = default;
    virtual ~IndexBuffer();

    IndexBuffer(unsigned int *indices, uint32_t size);
    IndexBuffer(std::vector<unsigned int> &indices);

    void Bind() const;
    void Unbind() const;

  private:
    unsigned int m_IndexBuffer;
};
}