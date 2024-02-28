#pragma once

namespace Engine
{
class ShadowMapFBO
{
  public:
    ShadowMapFBO();
    ~ShadowMapFBO();

    bool Init(unsigned int windowWidth, unsigned int windowHeight);
    void BindForWriting();
    void BindForReading(int textureUnit);

  private:
    unsigned int m_FBO;
    unsigned int m_ShadowMap;
};
}
