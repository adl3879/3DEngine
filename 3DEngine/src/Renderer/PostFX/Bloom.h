#pragma once

#include "ShaderManager.h"
//#include "Texture2D.h"
#include "BloomFBO.h"
#include "Framebuffer.h"

#include <vector>
#include <glm/glm.hpp>
#include <memory>

namespace Engine
{
class BloomRenderer
{
  public:
    BloomRenderer();
    ~BloomRenderer();
    bool Init(unsigned int windowWidth, unsigned int windowHeight);
    void Destroy();
    void RenderBloomTexture(unsigned int srcTexture, float filterRadius);
    unsigned int BloomTexture();

  private:
    void RenderDownsamples(unsigned int srcTexture);
    void RenderUpsamples(float filterRadius);

    bool mInit;
    BloomFBO mFBO;
    glm::ivec2 mSrcViewportSize;
    glm::vec2 mSrcViewportSizeFloat;
    Shader *mDownsampleShader;
    Shader *mUpsampleShader;
};

using BloomRendererRef = std::shared_ptr<BloomRenderer>;
} // namespace Engine
