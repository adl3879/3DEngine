#include "VertexArray.h"
#include "Buffer.h"
#include <iostream>

namespace Engine
{
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
        glVertexAttribPointer(i, element.Count, element.Type, element.Normalized,
                              layout.GetStride() * VertexBufferElement::GetSizeOfType(element.Type),
                              (const void *)(element.Offset * VertexBufferElement::GetSizeOfType(element.Type)));
        glEnableVertexAttribArray(i);
    }
    vb.Unbind();
}
} 