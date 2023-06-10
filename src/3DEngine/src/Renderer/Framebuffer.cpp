#include "Framebuffer.h"

#include <glad/glad.h>
#include <stdexcept>

namespace Engine
{
Framebuffer::Framebuffer(int width, int height) : m_Width(width), m_Height(height)
{
    //
    Invalidate();
}

Framebuffer::~Framebuffer()
{
    glDeleteFramebuffers(1, &m_RendererID);
    glDeleteTextures(1, &m_ColorAttachment);
    glDeleteTextures(1, &m_DepthAttachment);
}

void Framebuffer::Bind() const
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);
    glViewport(0, 0, m_Width, m_Height);
}

void Framebuffer::Unbind() const { glBindFramebuffer(GL_FRAMEBUFFER, 0); }

void Framebuffer::Invalidate()
{
    if (m_RendererID)
    {
        glDeleteFramebuffers(1, &m_RendererID);
        // glDeleteTextures(1, &m_ColorAttachment);
        // glDeleteTextures(1, &m_DepthAttachment);

        m_ColorAttachment = 0;
        m_DepthAttachment = 0;
    }

    glGenFramebuffers(1, &m_RendererID);
    glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);

    glGenTextures(1, &m_ColorAttachment);
    glBindTexture(GL_TEXTURE_2D, m_ColorAttachment);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Width, m_Height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_ColorAttachment, 0);

    glGenTextures(1, &m_DepthAttachment);
    glBindTexture(GL_TEXTURE_2D, m_DepthAttachment);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, m_Width, m_Height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8,
                 nullptr);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, m_DepthAttachment, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        throw std::runtime_error("Framebuffer is not complete!");

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
void Framebuffer::Resize(int width, int height)
{
    m_Width = width;
    m_Height = height;

    Invalidate();
}
} // namespace Engine