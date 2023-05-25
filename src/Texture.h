#pragma once

#include <stb_image.h>

#include "Shader.h"

class Texture
{
  public:
    Texture() = default;
    Texture(const std::string &src, const std::string &type, unsigned int slot);

    void TextureUnit(Shader &shader, const char *uniform, unsigned int unit);
    const std::string &GetType() const { return m_Type; }

    void Bind() const;
    void Unbind() const;
    void Delete() const { glDeleteTextures(1, &m_Texture); }

  private:
    unsigned int m_Texture;
    std::string m_Type;
    unsigned int m_Slot;
};
