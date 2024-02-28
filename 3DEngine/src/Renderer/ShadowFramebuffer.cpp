#include  "ShadowFramebuffer.h"

#include <glad/glad.h>
#include "Log.h"

namespace Engine
{
ShadowMapFBO::ShadowMapFBO() 
{
	m_FBO = 0;
	m_ShadowMap = 0;
	Init(2048, 2048);
}

ShadowMapFBO::~ShadowMapFBO() 
{
	if (m_FBO != 0) glDeleteFramebuffers(1, &m_FBO);
	if (m_ShadowMap != 0) glDeleteTextures(1, &m_ShadowMap);
}

bool ShadowMapFBO::Init(unsigned int windowWidth, unsigned int windowHeight) 
{
	glGenFramebuffers(1, &m_FBO);

	glGenTextures(1, &m_ShadowMap);
	glBindTexture(GL_TEXTURE_2D, m_ShadowMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, windowWidth, windowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_ShadowMap, 0);

	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
	{
		LOG_CORE_ERROR("Framebuffer is not complete!");
		return false;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	return true;
}

void ShadowMapFBO::BindForWriting() 
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
}

void ShadowMapFBO::BindForReading(int textureUnit) 
{
	glActiveTexture(GL_TEXTURE0 + textureUnit);
	glBindTexture(GL_TEXTURE_2D, m_ShadowMap);
}
} // namespace Engine
