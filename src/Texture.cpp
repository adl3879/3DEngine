#include "Texture.h"

Texture::Texture(const std::string &src) : m_Texture(0)
{
    // load image
    int width, height, channels;
    unsigned char *image = stbi_load(src.c_str(), &width, &height, &channels, STBI_rgb);

    // generate texture
    glGenTextures(1, &m_Texture);
    Bind();

    // Configures the type of algorithm that is used to make the image smaller or bigger
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // Configures the way the texture repeats (if it does at all)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(image);
    Unbind();
}

Texture::~Texture() { glDeleteTextures(1, &m_Texture); }

void Texture::Bind() const { glBindTexture(GL_TEXTURE_2D, m_Texture); }

void Texture::Unbind() const { glBindTexture(GL_TEXTURE_2D, 0); }
