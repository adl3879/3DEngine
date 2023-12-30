#include "Framebuffer.h"

#include <glad/glad.h>
#include "Log.h"

namespace Engine
{
Framebuffer::Framebuffer(bool hasRenderBuffer, glm::vec2 size)
{
    m_Textures = std::map<int, Texture2DRef>();
    m_Size = size;
    m_HasRenderBuffer = hasRenderBuffer;

    glGenFramebuffers(1, &m_FramebufferID);
    glBindFramebuffer(GL_FRAMEBUFFER, m_FramebufferID);

    // Create render buffer and attach to frame buffer.
    if (m_HasRenderBuffer)
    {
        glGenRenderbuffers(1, &m_RenderBuffer);
        glBindRenderbuffer(GL_RENDERBUFFER, m_RenderBuffer);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_Size.x, m_Size.y);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_RenderBuffer);
    }
    else m_RenderBuffer = -1;

    // Unbind
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // check if framebuffer is complete
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        LOG_CORE_ERROR("Framebuffer is not complete!");
    }
}

Framebuffer::~Framebuffer()
{
}

Texture2DRef Framebuffer::GetTexture(unsigned int attachment)
{
    return m_Textures[attachment];
}

void Framebuffer::SetTexture(Texture2DRef texture, unsigned int attachment)
{
    m_Textures[attachment] = texture;
    // Attach texture to the framebuffer.
    Bind();
    texture->AttachToFramebuffer(attachment);

    // Set draw buffer with dynamic amount of render target.
    // Surely, this can be optimized.
    int size = 0;
    std::vector<unsigned int> keys = std::vector<unsigned int>();
    for (auto &s : m_Textures)
    {
        if (s.first == GL_DEPTH_ATTACHMENT) continue;

        keys.push_back(s.first);
        size += 1;
    }

    if (size > 0)
        glDrawBuffers(size, &keys[0]);

    Unbind();
}

void Framebuffer::Clear()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Framebuffer::ClearAttachment(uint32_t index, int value)
{
    glClearTexImage(m_Textures[index]->GetRendererID(), 0, GL_RED_INTEGER, GL_INT, &value);
}

void Framebuffer::Bind()
{
    if (ResizeQueued) UpdateSize(m_Size);

    glBindFramebuffer(GL_FRAMEBUFFER, m_FramebufferID);
    glViewport(0, 0, m_Size.x, m_Size.y);
}

void Framebuffer::Unbind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::QueueResize(glm::vec2 size)
{
    if (size == m_Size) return;

    ResizeQueued = true;
    m_Size = size;
}

void Framebuffer::UpdateSize(glm::vec2 size)
{
    m_Size = size;
    ResizeQueued = false;

    // Delete frame buffer and render buffer.
    glDeleteFramebuffers(1, &m_FramebufferID);
    if (m_HasRenderBuffer)
        glDeleteRenderbuffers(1, &m_RenderBuffer);

    // New FBO and RBO.
    glGenFramebuffers(1, &m_FramebufferID);
    glBindFramebuffer(GL_FRAMEBUFFER, m_FramebufferID);

    // Recreate resized texture.
    for (auto &t : m_Textures)
    {
        t.second->Resize(size);
        SetTexture(t.second, t.first);
    }

    // Recreate render buffer 
    // TODO: move out render buffer.
    if (m_HasRenderBuffer)
    {
        glGenRenderbuffers(1, &m_RenderBuffer);
        glBindRenderbuffer(GL_RENDERBUFFER, m_RenderBuffer);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_Size.x, m_Size.y);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_RenderBuffer);
    }

    // Unbind.
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

int Framebuffer::ReadPixel(uint32_t attachment, const glm::vec2 coords)
{
    glReadBuffer(GL_COLOR_ATTACHMENT0 + attachment);
    int pixelData;
    glReadPixels((int)coords.x, (int)coords.y, 1, 1, GL_RED_INTEGER, GL_INT, &pixelData);
    return pixelData;
}

void Framebuffer::SetDrawBuffer(unsigned int draw)
{
    Bind();
    // glReadBuffer(draw);
    Unbind();
}

void Framebuffer::SetReadBuffer(unsigned int read)
{
    Bind();
    // glReadBuffer(read);
    Unbind();
}
} // namespace Engine
