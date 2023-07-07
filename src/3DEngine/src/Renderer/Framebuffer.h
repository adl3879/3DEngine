#pragma once

#include <glm/glm.hpp>
#include <vector>

namespace Engine
{
enum class FramebufferTextureFormat
{
    None = 0,

    // Color
    RGBA8,
    RED_INTEGER,

    // Depth/stencil
    DEPTH24STENCIL8,

    // Defaults
    Depth = DEPTH24STENCIL8
};

struct FramebufferTextureSpecification
{
    FramebufferTextureSpecification() = default;
    FramebufferTextureSpecification(FramebufferTextureFormat format) : TextureFormat(format) {}

    FramebufferTextureFormat TextureFormat = FramebufferTextureFormat::None;
};

struct FramebufferAttachmentSpecification
{
    FramebufferAttachmentSpecification() = default;
    FramebufferAttachmentSpecification(std::initializer_list<FramebufferTextureSpecification> attachments)
        : Attachments(attachments)
    {
    }

    std::vector<FramebufferTextureSpecification> Attachments;
};

struct FramebufferSpecification
{
    unsigned int Width = 0, Height = 0;
    FramebufferAttachmentSpecification Attachments;
    unsigned int Samples = 1;

    bool SwapChainTarget = false;
};

class Framebuffer
{
  public:
    Framebuffer(const FramebufferSpecification &spec);
    virtual ~Framebuffer();

    void Bind() const;
    void Unbind() const;
    void Invalidate();

    void Resize(int width, int height);
    int ReadPixel(unsigned int attachmentIndex, int x, int y) const;

    unsigned int GetColorAttachment(unsigned int index = 0) const
    {
        assert(index < m_ColorAttachments.size());
        return m_ColorAttachments[index];
    }
    // unsigned int GetDepthAttachment() const { return m_DepthAttachment; }

    const FramebufferSpecification &GetSpecification() const { return m_Specification; }

  public:
    void ClearAttachment(unsigned int attachmentIndex, int value);

  private:
    unsigned int m_RendererID = 0;
    FramebufferSpecification m_Specification;

    std::vector<FramebufferTextureSpecification> m_ColorAttachmentSpecifications;
    FramebufferTextureSpecification m_DepthAttachmentSpecification;

    std::vector<unsigned int> m_ColorAttachments;
    unsigned int m_DepthAttachment = 0;
};
} // namespace Engine