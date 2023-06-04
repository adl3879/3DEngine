#include "Buffer.h"

#include <glad/glad.h>

namespace Engine
{
VertexBuffer::VertexBuffer(float *vertices, uint32_t size)
{
    glGenBuffers(1, &m_VertexBuffer);
    Bind();
    glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
}

VertexBuffer::VertexBuffer(std::vector<Vertex> &vertices)
{
    glGenBuffers(1, &m_VertexBuffer);
    Bind();
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);
}

void VertexBuffer::Bind() const { glBindBuffer(GL_ARRAY_BUFFER, m_VertexBuffer); }

void VertexBuffer::Unbind() const { glBindBuffer(GL_ARRAY_BUFFER, 0); }

VertexBuffer::~VertexBuffer() { glDeleteBuffers(1, &m_VertexBuffer); }

IndexBuffer::IndexBuffer(unsigned int *indices, uint32_t size)
{
    glGenBuffers(1, &m_IndexBuffer);
    Bind();
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, indices, GL_STATIC_DRAW);
}

IndexBuffer::IndexBuffer(std::vector<unsigned int> &indices)
{
    glGenBuffers(1, &m_IndexBuffer);
    Bind();
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
}

IndexBuffer::~IndexBuffer() { glDeleteBuffers(1, &m_IndexBuffer); }

void IndexBuffer::Bind() const { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndexBuffer); }

void IndexBuffer::Unbind() const { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); }
} // namespace Engine