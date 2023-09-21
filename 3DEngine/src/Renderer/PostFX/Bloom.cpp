#include "Bloom.h"

#include "Renderer.h"

namespace Engine
{
const float DownsamplingScale = 0.5f;

Bloom::Bloom(unsigned int iteration) { m_Iteration = iteration; }

void Bloom::Init()
{
    if (!m_Source) return;

	m_FinalFB = std::make_shared<Framebuffer>(false, m_Size);
    m_FinalFB->SetTexture(std::make_shared<Texture2D>(ImageFormat::RGB16), GL_COLOR_ATTACHMENT0);

	m_ThresholdFB = std::make_shared<Framebuffer>(false, m_Size);
    m_ThresholdFB->SetTexture(std::make_shared<Texture2D>(ImageFormat::RGB16), GL_COLOR_ATTACHMENT0);

	m_DownSampleFB = std::vector<FramebufferRef>();
    m_UpSampleFB = std::vector<FramebufferRef>();
    m_HBlurFB = std::vector<FramebufferRef>();
    m_VBlurFB = std::vector<FramebufferRef>();

	glm::vec2 currentSize = m_Size / 2.0f;
	for (uint32_t i = 0; i < m_Iteration; i++)
	{
		FramebufferRef fb = std::make_shared<Framebuffer>(false, currentSize);
		fb->SetTexture(std::make_shared<Texture2D>(ImageFormat::RGB16), GL_COLOR_ATTACHMENT0);
		m_DownSampleFB.push_back(fb);

		currentSize *= DownsamplingScale;
	}

	for (uint32_t i = 0; i < m_Iteration; i++)
	{
		FramebufferRef fb = std::make_shared<Framebuffer>(false, currentSize);
		fb->SetTexture(std::make_shared<Texture2D>(ImageFormat::RGB16), GL_COLOR_ATTACHMENT0);
		m_UpSampleFB.push_back(fb);

		fb = std::make_shared<Framebuffer>(false, currentSize);
        fb->SetTexture(std::make_shared<Texture2D>(ImageFormat::RGB16), GL_COLOR_ATTACHMENT0);
		m_HBlurFB.push_back(fb);

		fb = std::make_shared<Framebuffer>(false, currentSize);
		fb->SetTexture(std::make_shared<Texture2D>(ImageFormat::RGB16), GL_COLOR_ATTACHMENT0);
		m_VBlurFB.push_back(fb);

		currentSize /= DownsamplingScale;
	}
}

int downsampleMip = 0;
int upSampleMip = 0;

void Bloom::Draw()
{
	if (!m_Source) return;

	Shader *shader = ShaderManager::GetShader("Resources/shaders/bloom");

	m_ThresholdFB->Bind();
	{
        m_ThresholdFB->Clear();
        shader->Bind();
        shader->SetUniform1i("u_Stage", 0);
        shader->SetUniform1f("u_Threshold", m_Threshold);
        m_Source->Bind(1);
        shader->SetUniform1i("u_Source", 1);

        Renderer::DrawQuad();
	}

	for (uint32_t i = 0; i < m_Iteration; i++)
    {
		m_DownSampleFB[i]->Bind();
		m_DownSampleFB[i]->Clear();

		shader->SetUniform1i("u_Stage", 1);

		Texture2DRef downsampleTexture = i == 0 ? m_ThresholdFB->GetTexture() : m_DownSampleFB[i - 1]->GetTexture();
		shader->SetUniform1i("u_Source", 1);
		// bind the downsample texture
		downsampleTexture->Bind(1);

		glm::vec2 size = i == 0 ? downsampleTexture->GetSize() : m_DownSampleFB[i]->GetTexture()->GetSize();
		shader->SetUniform2f("u_SourceSize", size);

		Renderer::DrawQuad();
		m_DownSampleFB[i]->Unbind();
    }

	for (uint32_t i = 0; i < m_Iteration; i++)
    {
        // Horizontal blur
        m_HBlurFB[i]->Bind();
        m_HBlurFB[i]->Clear();

        shader->SetUniform1i("u_Stage", 2);
        shader->SetUniform2f("u_BlurDirection", glm::vec2(0.0f, 1.0f));

        Texture2DRef blurTexture = m_DownSampleFB[m_Iteration - i - 1]->GetTexture();
        blurTexture->Bind(1);
        shader->SetUniform1i("u_Source", 1);
        shader->SetUniform2f("u_SourceSize", blurTexture->GetSize());

        Renderer::DrawQuad();
        m_HBlurFB[i]->Unbind();

        // Vertical blur
        m_VBlurFB[i]->Bind();
        m_VBlurFB[i]->Clear();

        shader->SetUniform1i("u_Stage", 2);
        shader->SetUniform2f("u_BlurDirection", glm::vec2(1.0f, 0.0f));

        m_HBlurFB[i]->GetTexture()->Bind(1);
        shader->SetUniform1i("u_Source", 1);
        shader->SetUniform2f("u_SourceSize", m_HBlurFB[i]->GetTexture()->GetSize());
        Renderer::DrawQuad();
        m_VBlurFB[i]->Unbind();

        // Upsampling
        m_UpSampleFB[i]->Bind();
        m_UpSampleFB[i]->Clear();
        if (i == 0)
        {
            shader->SetUniform1i("u_Stage", 3);
            shader->SetUniform1i("u_Source", 1);
            m_VBlurFB[i]->GetTexture()->Bind(1);
        }
        if (i > 0)
        {
            shader->SetUniform1i("u_Stage", 4);

            m_VBlurFB[i]->GetTexture()->Bind(1);
            shader->SetUniform1i("u_Source", 1);

            m_UpSampleFB[i - 1]->GetTexture()->Bind(2);
            shader->SetUniform1i("u_Source2", 2);
            shader->SetUniform2f("u_Source2Size", m_UpSampleFB[i]->GetTexture()->GetSize());
        }

        Renderer::DrawQuad();
        m_UpSampleFB[i]->Unbind();
    }

	m_FinalFB->Bind();
    m_FinalFB->Clear();
    {
        shader->SetUniform1i("u_Stage", 5);
        shader->SetUniform1i("u_Source", 1);
        shader->SetUniform1i("u_Source2", 2);
        m_UpSampleFB[m_Iteration - 1]->GetTexture()->Bind(1);
        m_Source->Bind(2);
        Renderer::DrawQuad();
    }

    m_FinalFB->Unbind();
}

void Bloom::Resize(glm::vec2 size)
{
    if (m_Size == size) return;
    m_Size = size;
    Init();
}

void Bloom::SetSource(Texture2DRef source) 
{
	if (m_Source == source) return;

	m_Source = source;
	m_Size.x = m_Source->GetWidth();
	m_Size.y = m_Source->GetHeight();
	Init();
}
} // namespace Engine
