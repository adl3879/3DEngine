#include "VertexArray.h"

#include <glad/glad.h>

#include "Buffer.h"
#include "Log.h"
#include <iostream>

namespace Engine
{
static GLenum ShaderDataTypeToOpenGLBaseType(ShaderDataType type)
{
    switch (type)
    {
        case ShaderDataType::Float: return GL_FLOAT;
        case ShaderDataType::Float2: return GL_FLOAT;
        case ShaderDataType::Float3: return GL_FLOAT;
        case ShaderDataType::Float4: return GL_FLOAT;
        case ShaderDataType::Mat3: return GL_FLOAT;
        case ShaderDataType::Mat4: return GL_FLOAT;
        case ShaderDataType::Int: return GL_INT;
        case ShaderDataType::Int2: return GL_INT;
        case ShaderDataType::Int3: return GL_INT;
        case ShaderDataType::Int4: return GL_INT;
        case ShaderDataType::Bool: return GL_BOOL;
        case ShaderDataType::None: return 0;
    }
    return 0;
}

VertexArray::VertexArray()
{
    glGenVertexArrays(1, &m_VertexArray);
    Bind();
}

void VertexArray::Bind() const { glBindVertexArray(m_VertexArray); }

void VertexArray::Unbind() const { glBindVertexArray(0); }

void VertexArray::Delete() const { glDeleteVertexArrays(1, &m_VertexArray); }

void VertexArray::AddBuffer(const VertexBuffer &vb, const VertexBufferLayout &layout)
{
    vb.Bind();
    const auto &elements = layout.GetElements();
    unsigned int offset = 0;
    for (unsigned int i = 0; i < elements.size(); i++)
    {
        const auto &element = elements[i];
        switch (element.Type)
        {
            case ShaderDataType::Float:
            case ShaderDataType::Float2:
            case ShaderDataType::Float3:
            case ShaderDataType::Float4:
            {
                glEnableVertexAttribArray(i);
                glVertexAttribPointer(i, element.GetComponentCount(), ShaderDataTypeToOpenGLBaseType(element.Type),
                                      element.Normalized, layout.GetStride() * sizeof(float),
                                      (const void *)(element.Offset * sizeof(float)));
            }
            break;
            case ShaderDataType::Int:
            case ShaderDataType::Int2:
            case ShaderDataType::Int3:
            case ShaderDataType::Int4:
            case ShaderDataType::Bool:
            {
                glEnableVertexAttribArray(i);
                glVertexAttribIPointer(i, element.GetComponentCount(), ShaderDataTypeToOpenGLBaseType(element.Type),
                                       layout.GetStride() * sizeof(float),
                                       (const void *)(element.Offset * sizeof(float)));
            }
        }
    }
    vb.Unbind();
}

} // namespace Engine