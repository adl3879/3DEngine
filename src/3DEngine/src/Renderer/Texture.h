#pragma once

#include <stb_image.h>
#include <vector>
#include <memory>

#include "Shader.h"
#include "Buffer.h"
#include "Asset/Asset.h"

namespace Engine
{
enum class ImageFormat
{
    None = 0,
    R8,
    RGB8,
    RGBA8,
    RGBA32F
};

struct TextureSpecification
{
    uint32_t Width = 1;
    uint32_t Height = 1;
    ImageFormat Format = ImageFormat::RGBA8;
    bool GenerateMips = true;
};

class Texture : public Asset
{
  public:
    virtual ~Texture() = default;

    virtual const TextureSpecification &GetSpecification() const = 0;

    virtual uint32_t GetWidth() const = 0;
    virtual uint32_t GetHeight() const = 0;
    virtual uint32_t GetRendererID() const = 0;

    virtual void SetData(Buffer data) = 0;

    virtual void Bind(uint32_t slot = 0) const = 0;
    virtual void Unbind() const = 0;
};

class Texture2D : public Texture
{
  public:
    Texture2D(const TextureSpecification &specification, Buffer data);
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

  private:
    TextureSpecification m_Specification;
    unsigned int m_RendererID = 0;
    unsigned int m_InternalFormat, m_DataFormat;
};

using Texture2DRef = std::shared_ptr<Texture2D>;
} // namespace Engine