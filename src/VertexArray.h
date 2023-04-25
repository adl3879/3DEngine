#pragma once

#include "Shader.h"
#include "Buffer.h"

class VertexArray
{
  public:
    VertexArray();
    virtual ~VertexArray();

    void Bind() const;
    void Unbind() const;

    void AddBuffer(const VertexBuffer &vb, const VertexBufferLayout &layout);

  private:
    unsigned int m_VertexArray;
};
