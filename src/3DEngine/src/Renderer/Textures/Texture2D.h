#pragma once

#include "Texture.h"

namespace Engine
{
class Texture2D : public Texture
{
  public:
    Texture2D(const TextureSpecification &specification, Buffer data);
    Texture2D(const TextureSpecification &specification);
    ~Texture2D();

    virtual void SetData(Buffer data) override;
    virtual void Bind(uint32_t slot = 0) const override;
    virtual void Unbind() const override;

    virtual uint32_t GetWidth() const override { return m_Specification.Width; }
    virtual uint32_t GetHeight() const override { return m_Specification.Height; }
    virtual uint32_t GetRendererID() const override { return m_RendererID; }
    virtual const TextureSpecification &GetSpecification() const override { return m_Specification; }

    static AssetType GetStaticType() { return AssetType::Texture2D; }
    virtual AssetType GetType() const override { return GetStaticType(); }

    void Resize(glm::vec2 size);
    void AttachToFramebuffer(uint32_t attachment);

  private:
    TextureSpecification m_Specification;
    unsigned int m_RendererID = 0;
    unsigned int m_InternalFormat, m_DataFormat;
};

using Texture2DRef = std::shared_ptr<Texture2D>;
} // namespace Engine