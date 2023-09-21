#pragma once

#include <vector>
#include <memory>
#include <glm/glm.hpp>

#include "Shader.h"
#include "Buffer.h"
#include "Asset.h"

namespace Engine
{
enum class ImageFormat
{
    None = 0,
    R8,
    RGB8,
    RGB16,
    RGBA8,
    RGBA32F,
    RED_INTEGER,

    // Depth/stencil formats
    Depth,
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
	Texture() = default;
    virtual ~Texture() = default;

    virtual const TextureSpecification &GetSpecification() const = 0;

    virtual glm::vec2 GetSize() const = 0;
    virtual uint32_t GetWidth() const = 0;
    virtual uint32_t GetHeight() const = 0;
    virtual uint32_t GetRendererID() const = 0;

    virtual void SetData(Buffer data) = 0;

    virtual void Bind(uint32_t slot = 0) const = 0;
    virtual void Unbind() const = 0;
};
} // namespace Engine
