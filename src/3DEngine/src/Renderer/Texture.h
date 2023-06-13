#pragma once

#include <stb_image.h>
#include <vector>

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
    void Delete() const;

  private:
    unsigned int m_Texture;
    std::string m_Type;
    std::string m_Path;
    unsigned int m_Slot;
};

class Texture3D
{
  public:
    Texture3D() = default;
    Texture3D(const std::vector<std::string> &faces);

    void Bind() const;
    void Unbind() const;

  private:
    unsigned int m_Texture;
};
} // namespace Engine