#pragma once

#include <stb_image.h>

#include "Shader.h"

class Texture
{
  public:
    Texture() = default;
    Texture(const std::string &src);
    virtual ~Texture();

    void Bind() const;
    void Unbind() const;

  private:
    unsigned int m_Texture;
    const char *m_Type;
};
