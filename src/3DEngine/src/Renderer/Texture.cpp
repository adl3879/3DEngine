#include "Texture.h"

#include <glad/glad.h>

#include <stdexcept>

namespace Engine
{
Texture::Texture(const std::string &src, const std::string &type, unsigned int slot)
    : m_Texture(0), m_Type(type), m_Slot(slot), m_Path(src)
{
    // load image
    int width, height, channels;
    unsigned char *image = stbi_load(src.c_str(), &width, &height, &channels, 0);

    // generate texture
    glGenTextures(1, &m_Texture);

    // Assigns the texture to the current texture slot
    glActiveTexture(GL_TEXTURE0 + slot);
    m_Slot = slot;

    Bind();

    // Configures the type of algorithm that is used to make the image smaller or bigger
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // Configures the way the texture repeats (if it does at all)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // Check what type of color channels the texture has and load it accordingly
    if (channels == 4)
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
    else if (channels == 3)
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    else if (channels == 1)
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, image);
    else
        throw std::invalid_argument("Automatic Texture type recognition failed");

    // Generate MipMaps
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(image);
    Unbind();
}

void Texture::TextureUnit(Shader &shader, const char *uniform, unsigned int unit)
{
    GLuint texUni = glGetUniformLocation(shader.GetProgram(), uniform);
    shader.Use();
    // Sets the value of the uniform
    glUniform1i(texUni, unit);
}

void Texture::Bind() const
{
    glActiveTexture(GL_TEXTURE0 + m_Slot);
    glBindTexture(GL_TEXTURE_2D, m_Texture);
}

void Texture::Unbind() const { glBindTexture(GL_TEXTURE_2D, 0); }

void Texture::Delete() const { glDeleteTextures(1, &m_Texture); }
} // namespace Engine