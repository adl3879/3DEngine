#pragma once

#include "Texture.h"

namespace Engine
{
class TextureHDRI : public Texture
{
  public:
	TextureHDRI() = default;
    TextureHDRI(const TextureSpecification &specification, float *data);
    ~TextureHDRI();

    virtual void SetData(Buffer data) override;
    virtual void Bind(uint32_t slot = 0) const override;
    virtual void Unbind() const override;

    virtual uint32_t GetWidth() const override { return m_Specification.Width; }
    virtual uint32_t GetHeight() const override { return m_Specification.Height; }
    virtual uint32_t GetRendererID() const override { return m_RendererID; }
    virtual const TextureSpecification &GetSpecification() const override { return m_Specification; }

    static AssetType GetStaticType() { return AssetType::TextureHDRI; }
    virtual AssetType GetType() const override { return GetStaticType(); }

  private:
    TextureSpecification m_Specification;
    unsigned int m_RendererID = 0;
    unsigned int m_InternalFormat, m_DataFormat;
};
using TextureHDRIRef = std::shared_ptr<TextureHDRI>;
}
