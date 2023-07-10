#include "VertexArray.h"

#include <glad/glad.h>

namespace Engine
{
void VertexArray::Init() noexcept { glGenVertexArrays(1, &m_VAO); }

void VertexArray::AttachBuffer(const BufferType &type, const int size, const DrawMode &mode, const void *data) noexcept
{
    GLuint buffer;
    glGenBuffers(1, &buffer);

    glBindBuffer(type, buffer);
    glBufferData(type, size, data, mode);
}

void VertexArray::Bind() const noexcept { glBindVertexArray(m_VAO); }

void VertexArray::Unbind() const noexcept { glBindVertexArray(0); }

void VertexArray::EnableAttribute(const uint32_t index, const int size, const uint32_t offset,
                                  const void *data) noexcept
{
    glEnableVertexAttribArray(index);
    glVertexAttribPointer(index, size, GL_FLOAT, GL_FALSE, offset, data);
}

void VertexArray::SetBufferSubData(const BufferType &type, const uint32_t size, const uint32_t offset,
                                   const void *data) noexcept
{
    glBufferSubData(type, offset, size, data);
}

void VertexArray::Delete() noexcept { glDeleteVertexArrays(1, &m_VAO); }
} // namespace Engine