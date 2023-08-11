#include "Texture.h"

#include <glad/glad.h>

namespace Engine
{
namespace Utils
{
static GLenum HazelImageFormatToGLDataFormat(ImageFormat format)
{
    switch (format)
    {
        case ImageFormat::RGB8: return GL_RGB;
        case ImageFormat::RGBA8: return GL_RGBA;
        default: break;
    }
    return 0;
}

static GLenum HazelImageFormatToGLInternalFormat(ImageFormat format)
{
    switch (format)
    {
        case ImageFormat::RGB8: return GL_RGB8;
        default: break;
        case ImageFormat::RGBA8: return GL_RGBA8;
    }
    return 0;
}
} // namespace Utils

Texture2D::Texture2D(const TextureSpecification &specification, Buffer data) : m_Specification(specification)
{
    m_InternalFormat = Utils::HazelImageFormatToGLInternalFormat(m_Specification.Format);
    m_DataFormat = Utils::HazelImageFormatToGLDataFormat(m_Specification.Format);

    glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
    glTextureStorage2D(m_RendererID, 1, m_InternalFormat, m_Specification.Width, m_Specification.Height);

    glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);

    if (data) SetData(data);
}

Texture2D::~Texture2D()
{ /**glDeleteTextures(1, &m_RendererID);**/
}

void Texture2D::SetData(Buffer data)
{
    uint32_t bpp = m_DataFormat == GL_RGBA ? 4 : 3;
    glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Specification.Width, m_Specification.Height, m_DataFormat,
                        GL_UNSIGNED_BYTE, data.Data);
}

void Texture2D::Bind(uint32_t slot) const
{
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, m_RendererID);
}

void Texture2D::Unbind() const { glBindTexture(GL_TEXTURE_2D, 0); }
} // namespace Engine