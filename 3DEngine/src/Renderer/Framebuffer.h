#pragma once

#include <glm/glm.hpp>
#include <map>
#include <memory>

#include "Texture2D.h"

#define GL_DEPTH_ATTACHMENT 0x8D00

#define GL_COLOR_ATTACHMENT0 0x8CE0
#define GL_COLOR_ATTACHMENT1 0x8CE1
#define GL_COLOR_ATTACHMENT2 0x8CE2
#define GL_COLOR_ATTACHMENT3 0x8CE3
#define GL_COLOR_ATTACHMENT4 0x8CE4

namespace Engine
{
enum class Attachment
{
    DEPTH = 0x8D00,
    COLOR_0 = 0x8CE0,
    COLOR_1 = COLOR_0 + 1,
    COLOR_2 = COLOR_0 + 2,
};

class Framebuffer
{
public:
    Framebuffer(bool hasRenderBuffer, glm::vec2 size);
    ~Framebuffer();

    Texture2DRef GetTexture(unsigned int attachment = 0x8CE0);
    void SetTexture(Texture2DRef texture, unsigned int attachment = 0x8CE0);

    void Bind();
    void Unbind();
    void Clear();
    void ClearAttachment(uint32_t index, int value);
    void QueueResize(glm::vec2 size);
    glm::vec2 GetSize() const { return m_Size; }
    void SetSize(const glm::vec2 &size) { m_Size = size; }
    void UpdateSize(glm::vec2 size);

    int ReadPixel(uint32_t attachment, const glm::vec2 coords);
    void SetDrawBuffer(unsigned int draw);
    void SetReadBuffer(unsigned int read);

private:
    unsigned int m_FramebufferID;
    unsigned int m_RenderBuffer;

    glm::vec2 m_Size;
    bool ResizeQueued = false;

    std::map<int, Texture2DRef> m_Textures;
    Texture2DRef m_Texture;
    bool m_HasRenderBuffer = false;
};

using FramebufferRef = std::shared_ptr<Framebuffer>;
} // namespace Engine
