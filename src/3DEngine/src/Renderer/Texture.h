#pragma once

#include <stb_image.h>

#include "Shader.h"

namespace Engine
{
class Texture
{
  public:
    Texture() = default;
    Texture(const std::string &src, const std::string &type, unsigned int slot);

    void TextureUnit(Shader &shader, const char *uniform, unsigned int unit);
    const std::string &GetType() const { return m_Type; }
    const std::string &GetPath() const { return m_Path; }

    void Bind() const;
    void Unbind() const;
    void Delete() const { glDeleteTextures(1, &m_Texture); }

  private:
    unsigned int m_Texture;
    std::string m_Type;
    std::string m_Path;
    unsigned int m_Slot;
};
}