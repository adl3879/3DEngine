#include "VertexArray.h"

VertexArray::VertexArray()
{
    glGenVertexArrays(1, &m_VertexArray);
    Bind();
}

VertexArray::~VertexArray() { glDeleteVertexArrays(1, &m_VertexArray); }

void VertexArray::Bind() const { glBindVertexArray(m_VertexArray); }

void VertexArray::Unbind() const { glBindVertexArray(0); }
