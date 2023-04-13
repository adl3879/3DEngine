#pragma once

#include "Shader.h"

class VertexBuffer
{
  public:
    VertexBuffer() = default;
    VertexBuffer(float *vertices, uint32_t size);
    virtual ~VertexBuffer();

    void Bind() const;
    void Unbind() const;

  private:
    unsigned int m_VertexBuffer;
};

class IndexBuffer
{
  public:
    IndexBuffer() = default;
    IndexBuffer(unsigned int *indices, uint32_t size);
    virtual ~IndexBuffer();

    void Bind() const;
    void Unbind() const;

  private:
    unsigned int m_IndexBuffer;
};
