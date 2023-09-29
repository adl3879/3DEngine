#include "Bloom2.h"

#include "Renderer.h"

namespace Engine
{
const float DownsamplingScale = 0.4f;

Bloom::Bloom(unsigned int iteration) { m_Iteration = iteration; }

void Bloom::SetSource(Texture2DRef source)
{
    if (m_Source == source) return;

    m_Source = source;
    m_Size = source->GetSize();
    Init();
}

void Bloom::Init()
{
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
        shader->SetUniform1i("u_Source", 0);
        m_Source->Bind(0);

        Renderer::DrawQuad();
		//m_ThresholdFB->Unbind();
    }

    for (uint32_t i = 0; i < m_Iteration; i++)
    {
        m_DownSampleFB[i]->Bind();
        m_DownSampleFB[i]->Clear();
		
        shader->SetUniform1i("u_Stage", 1);

        Texture2DRef downsampleTexture = i == 0 ? m_ThresholdFB->GetTexture() : m_DownSampleFB[i - 1]->GetTexture();
        shader->SetUniform1i("u_Source", 0);
		downsampleTexture->Bind(0);

        auto size = i == 0 ? downsampleTexture->GetSize() : m_DownSampleFB[i]->GetTexture()->GetSize();
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
        shader->SetUniform2f("u_BlurDirection", {0.0f, 1.0f});

        Texture2DRef blurTexture = m_DownSampleFB[m_Iteration - i - 1]->GetTexture();
        blurTexture->Bind(0);
        shader->SetUniform1i("u_Source", 0);
        shader->SetUniform2f("u_SourceSize", {(float)blurTexture->GetWidth(), (float)blurTexture->GetHeight()});

        Renderer::DrawQuad();
        m_HBlurFB[i]->Unbind();

        // Vertical blur
        m_VBlurFB[i]->Bind();
        m_VBlurFB[i]->Clear();

        shader->SetUniform1i("u_Stage", 2);
        shader->SetUniform2f("u_BlurDirection", {1.0f, 0.0f});

        m_HBlurFB[i]->GetTexture()->Bind(0);
        shader->SetUniform1i("u_Source", 0);
        shader->SetUniform2f("u_SourceSize", {(float)m_HBlurFB[i]->GetTexture()->GetWidth(),
                                              (float)m_HBlurFB[i]->GetTexture()->GetHeight()});

        Renderer::DrawQuad();
        m_VBlurFB[i]->Unbind();

        // Upsampling
        m_UpSampleFB[i]->Bind();
        m_UpSampleFB[i]->Clear();
        if (i == 0)
        {
            shader->SetUniform1i("u_Stage", 3);
            shader->SetUniform1i("u_Source", 0);
            m_VBlurFB[i]->GetTexture()->Bind(0);
        }
        if (i > 0)
        {
            shader->SetUniform1i("u_Stage", 4);

            m_VBlurFB[i]->GetTexture()->Bind(0);
            shader->SetUniform1i("u_Source", 0);

            m_UpSampleFB[i - 1]->GetTexture()->Bind(1);
            shader->SetUniform1i("u_Source2", 1);
            shader->SetUniform2f("u_Source2Size", {(float)m_UpSampleFB[i]->GetTexture()->GetWidth(),
                                                   (float)m_UpSampleFB[i]->GetTexture()->GetHeight()});
        }
        Renderer::DrawQuad();
        m_UpSampleFB[i]->Unbind();
    }
    m_FinalFB->Bind();
    m_FinalFB->Clear();
    {
        shader->SetUniform1i("u_Stage", 5);
        shader->SetUniform1i("u_Source", 0);
        shader->SetUniform1i("u_Source2", 1);
        m_UpSampleFB[m_Iteration - 1]->GetTexture()->Bind(0);
        m_Source->Bind(1);
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
}
