#pragma once

#include "Buffer.h"

namespace Engine
{
class VertexArray
{
  public:
    VertexArray();

    void Bind() const;
    void Unbind() const;
    void Delete() const;

    void AddBuffer(const VertexBuffer &vb, const VertexBufferLayout &layout);

  private:
    unsigned int m_VertexArray;
};
}