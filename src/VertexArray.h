#pragma once

#include "Shader.h"
#include "Buffer.h"

class VertexArray
{
  public:
    VertexArray();

    void Bind() const;
    void Unbind() const;
    void Delete() const { glDeleteVertexArrays(1, &m_VertexArray); }

    void AddBuffer(const VertexBuffer &vb, const VertexBufferLayout &layout);

  private:
    unsigned int m_VertexArray;
};
