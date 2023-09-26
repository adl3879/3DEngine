#include "Texture2D.h"

#include <glad/glad.h>

namespace Engine
{
namespace Utils
{
static GLenum ImageFormatToGLDataFormat(ImageFormat format)
{
    switch (format)
    {
        case ImageFormat::R8: return GL_RED;
        case ImageFormat::RGB8: return GL_RGB;
        case ImageFormat::RGBA8: return GL_RGBA;
        case ImageFormat::RGB16: return GL_RGBA;
        case ImageFormat::RED_INTEGER: return GL_RED_INTEGER;
        case ImageFormat::Depth: return GL_DEPTH_COMPONENT;
        default: break;
    }
    return 0;
}

static GLenum ImageFormatToGLInternalFormat(ImageFormat format)
{
    switch (format)
    {
        case ImageFormat::R8: return GL_RED;
        case ImageFormat::RGB8: return GL_RGB8;
        case ImageFormat::RGBA8: return GL_RGBA8;
        case ImageFormat::RGB16: return GL_RGB16F;
        case ImageFormat::RED_INTEGER: return GL_R32I;
        case ImageFormat::Depth: return GL_DEPTH_COMPONENT;
        default: break;
    }
    return 0;
}

static GLenum ImageFormatToGLDataType(ImageFormat format)
{
    switch (format)
    {
        case ImageFormat::R8: return GL_UNSIGNED_BYTE;
        case ImageFormat::RGB8: return GL_UNSIGNED_BYTE;
        case ImageFormat::RGBA8: return GL_UNSIGNED_BYTE;
        case ImageFormat::RGB16: return GL_FLOAT;
        case ImageFormat::RED_INTEGER: return GL_INT;
        case ImageFormat::Depth: return GL_FLOAT;
        default: break;
    }
    return 0;
}
} // namespace Utils

Texture2D::Texture2D()
{
}

Texture2D::Texture2D(const TextureSpecification &specification, Buffer data) : m_Specification(specification)
{
    m_InternalFormat = Utils::ImageFormatToGLInternalFormat(m_Specification.Format);
    m_DataFormat = Utils::ImageFormatToGLDataFormat(m_Specification.Format);
    m_DataType = Utils::ImageFormatToGLDataType(m_Specification.Format);

    glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
    glTextureStorage2D(m_RendererID, 1, m_InternalFormat, m_Specification.Width, m_Specification.Height);

    glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);

    if (data) SetData(data);
}

Texture2D::Texture2D(const TextureSpecification &specification)
{
    m_Specification = specification;
    m_InternalFormat = Utils::ImageFormatToGLInternalFormat(m_Specification.Format);
    m_DataFormat = Utils::ImageFormatToGLDataFormat(m_Specification.Format);
    m_DataType = Utils::ImageFormatToGLDataType(m_Specification.Format);

    glGenTextures(1, &m_RendererID);
    glBindTexture(GL_TEXTURE_2D, m_RendererID);
    glTexImage2D(GL_TEXTURE_2D, 0, m_InternalFormat, m_Specification.Width, m_Specification.Height, 0, m_DataFormat,
                 m_DataType, nullptr);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
}

Texture2D::Texture2D(ImageFormat format) 
{
    m_Specification = TextureSpecification{.Format = format};
    m_InternalFormat = Utils::ImageFormatToGLInternalFormat(m_Specification.Format);
    m_DataFormat = Utils::ImageFormatToGLDataFormat(m_Specification.Format);
    m_DataType = Utils::ImageFormatToGLDataType(m_Specification.Format);

    glGenTextures(1, &m_RendererID);
    glBindTexture(GL_TEXTURE_2D, m_RendererID);
    glTexImage2D(GL_TEXTURE_2D, 0, m_InternalFormat, m_Specification.Width, m_Specification.Height, 0, m_DataFormat,
                 m_DataType, nullptr);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
}

Texture2D::~Texture2D() {}

void Texture2D::SetData(Buffer data) 
{
    uint32_t bpp = m_DataFormat == GL_RGBA ? 4 : 3;
	assert(data.Size = m_Specification.Width * m_Specification.Height * bpp);
    glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Specification.Width, m_Specification.Height, m_DataFormat, m_DataType,
                        data.Data);
}

void Texture2D::Bind(uint32_t slot) const
{
    // bind at slot
	//glBindTextureUnit(slot, m_RendererID);
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D, m_RendererID);
}

void Texture2D::Unbind() const { glBindTexture(GL_TEXTURE_2D, 0); }

void Texture2D::Resize(glm::vec2 size)
{
    glDeleteTextures(1, &m_RendererID);
    m_Specification.Width = size.x;
    m_Specification.Height = size.y;

    glGenTextures(1, &m_RendererID);
    glBindTexture(GL_TEXTURE_2D, m_RendererID);
    glTexImage2D(GL_TEXTURE_2D, 0, m_InternalFormat, size.x, size.y, 0, m_DataFormat, m_DataType, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void Texture2D::AttachToFramebuffer(uint32_t attachment)
{
    glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, m_RendererID, 0);
}
} // namespace Engine
