#include "Texture2DArray.h"

#include <glad/glad.h>

namespace Engine
{
Texture2DArray::Texture2DArray() {}

Texture2DArray::Texture2DArray(ImageFormat format, int width, int height) 
{
    m_Specification = TextureSpecification{.Format = format};
    m_Specification.Width = width;
    m_Specification.Height = height;

	Resize({width, height});
}

Texture2DArray::~Texture2DArray() {}

void Texture2DArray::SetData(Buffer data) 
{
	// only required for textures loaded from disk
}

void Texture2DArray::Bind(uint32_t slot) const
{
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D_ARRAY, m_RendererID);
}

void Texture2DArray::Unbind() const 
{
	glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
}

void Texture2DArray::Resize(glm::vec2 size) 
{ 
	if (m_RendererID != 0) glDeleteTextures(1, &m_RendererID);

	//! TEMPORARILY WORKS ONLY FOR CASCADED SHADOW MAPS
    int depthMapResolution = 4096;
    float cameraFarPlane = 500.0f;
    std::vector<float> shadowCascadeLevels{cameraFarPlane / 50.0f, cameraFarPlane / 25.0f, cameraFarPlane / 10.0f,
                                            cameraFarPlane / 2.0f};

    glGenTextures(1, &m_RendererID);
    glBindTexture(GL_TEXTURE_2D_ARRAY, m_RendererID);
    glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_DEPTH_COMPONENT32F, depthMapResolution, depthMapResolution,
                    int(shadowCascadeLevels.size()) + 1, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);

    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    constexpr float bordercolor[] = {1.0f, 1.0f, 1.0f, 1.0f};
    glTexParameterfv(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_BORDER_COLOR, bordercolor);
}

void Texture2DArray::AttachToFramebuffer(uint32_t attachment) 
{
    glFramebufferTexture(GL_FRAMEBUFFER, attachment, m_RendererID, 0);
}
} // namespace Engine
