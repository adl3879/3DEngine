#pragma once

#include <vector>

#include <glm/fwd.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Engine
{
enum class ShaderDataType
{
    None = 0,
    Float,
    Float2,
    Float3,
    Float4,
    Mat3,
    Mat4,
    Int,
    Int2,
    Int3,
    Int4,
    Bool
};

static unsigned int GetSizeOfType(ShaderDataType type)
{
    switch (type)
    {
        case ShaderDataType::Float: return 4;
        case ShaderDataType::Float2: return 4 * 2;
        case ShaderDataType::Float3: return 4 * 3;
        case ShaderDataType::Float4: return 4 * 4;
        case ShaderDataType::Mat3: return 4 * 3 * 3;
        case ShaderDataType::Mat4: return 4 * 4 * 4;
        case ShaderDataType::Int: return 4;
        case ShaderDataType::Int2: return 4 * 2;
        case ShaderDataType::Int3: return 4 * 3;
        case ShaderDataType::Int4: return 4 * 4;
        case ShaderDataType::Bool: return 1;
        case ShaderDataType::None: return 0;
    }
}

struct VertexBufferElement
{
  public:
    ShaderDataType Type;
    unsigned int Count;
    size_t Offset;
    unsigned char Normalized;

    uint32_t GetComponentCount() const
    {
        switch (Type)
        {
            case ShaderDataType::Float: return 1;
            case ShaderDataType::Float2: return 2;
            case ShaderDataType::Float3: return 4;
            case ShaderDataType::Float4: return 4;
            case ShaderDataType::Mat3: return 3; // 3* float3
            case ShaderDataType::Mat4: return 4; // 4* float4
            case ShaderDataType::Int: return 1;
            case ShaderDataType::Int2: return 2;
            case ShaderDataType::Int3: return 3;
            case ShaderDataType::Int4: return 4;
            case ShaderDataType::Bool: return 1;
            case ShaderDataType::None: return 0;
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
    glm::vec3 Color = glm::vec3(1.0f);
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
} // namespace Engine