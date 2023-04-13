#pragma once

#include "Shader.h"

class VertexArray
{
  public:
    VertexArray();
    virtual ~VertexArray();

    void Bind() const;
    void Unbind() const;

  private:
    unsigned int m_VertexArray;
};
