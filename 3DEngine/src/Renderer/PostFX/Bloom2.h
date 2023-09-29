#pragma once

#include "ShaderManager.h"
#include "Texture2D.h"
#include "Framebuffer.h"

namespace Engine
{
class Bloom
{
    const float MAX_THRESHOLD = 500.0f;
    const float MIN_THRESHOLD = 0.0f;
    const float MAX_BLUR = 50.0f;
    const float MIN_BLUR = 0.0f;
    const unsigned int MAX_ITERATION = 5;
    const unsigned int MIN_ITERATION = 1;

  private:
    float m_Threshold = 2.3f;
    float m_BlurAmount = 12.0f;
    uint32_t m_Iteration = 4;
    glm::vec2 m_Size;

    Texture2DRef m_Source;
    FramebufferRef m_FinalFB;
    FramebufferRef m_ThresholdFB;
    std::vector<FramebufferRef> m_DownSampleFB;
    std::vector<FramebufferRef> m_HBlurFB;
    std::vector<FramebufferRef> m_VBlurFB;
    std::vector<FramebufferRef> m_UpSampleFB;

  public:
    Bloom() = default;
    Bloom(unsigned int iteration = 4);

    void Init();
    void Draw();

    void Resize(glm::vec2 size);

    inline float GetThreshold() const { return m_Threshold; }
    void SetThreshold(float threshold)
    {
        if (threshold <= MAX_THRESHOLD && threshold >= MIN_THRESHOLD) m_Threshold = threshold;
    }

    void SetIteration(unsigned int iteration)
    {
        if (iteration > MAX_ITERATION || iteration < MIN_ITERATION) return;

        m_Iteration = iteration;

        Init();
    };
    inline unsigned int GetIteration() const { return m_Iteration; }

    void SetSource(Texture2DRef source);
    Texture2DRef GetOutput() const { return m_FinalFB->GetTexture(); }
};

using BloomRef = std::shared_ptr<Bloom>;
} // namespace Engine
