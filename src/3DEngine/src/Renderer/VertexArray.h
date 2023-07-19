#pragma once

#include <stdint.h>

#define GL_ARRAY_BUFFER 0x8892
#define GL_ELEMENT_ARRAY_BUFFER 0x8893

#define GL_STATIC_DRAW 0x88E4
#define GL_DYNAMIC_DRAW 0x88E8
#define GL_STREAM_DRAW 0x88E0

namespace Engine
{
enum BufferType : int
{
    ARRAY = GL_ARRAY_BUFFER,
    ELEMENT = GL_ELEMENT_ARRAY_BUFFER,
};

enum DrawMode : int
{
    STATIC = GL_STATIC_DRAW,
    DYNAMIC = GL_DYNAMIC_DRAW,
    STREAM = GL_STREAM_DRAW
};

class VertexArray
{
  public:
    VertexArray() = default;
    virtual ~VertexArray() = default;

    uint32_t GetID() const noexcept { return m_VAO; }

    void Init() noexcept;
    void AttachBuffer(const BufferType &type, const int size, const DrawMode &mode, const void *data) noexcept;
    void Bind() const noexcept;
    void Unbind() const noexcept;
    void EnableAttribute(const uint32_t index, const int size, const uint32_t offset, const void *data) noexcept;
    void SetBufferSubData(const BufferType &type, const uint32_t offset, const uint32_t size,
                          const void *data) noexcept;

    void Delete() noexcept;

  private:
    uint32_t m_VAO = 0;
};
} // namespace Engine