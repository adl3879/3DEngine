#pragma once

#include <glm/glm.hpp>

namespace Engine
{
class Framebuffer
{
  public:
    Framebuffer(int width, int height);
    virtual ~Framebuffer();

    void Bind() const;
    void Unbind() const;
    void Invalidate();

    void Resize(int width, int height);

    unsigned int GetColorAttachment() const { return m_ColorAttachment; }
    unsigned int GetDepthAttachment() const { return m_DepthAttachment; }
    unsigned int GetWidth() const { return m_Width; }
    unsigned int GetHeight() const { return m_Height; }

  private:
    unsigned int m_RendererID = 0;
    unsigned int m_ColorAttachment = 0, m_DepthAttachment = 0;
    unsigned int m_Width, m_Height;
};
} // namespace Engine