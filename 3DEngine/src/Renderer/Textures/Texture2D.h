#pragma once

#include "Texture.h"

#include <glm/glm.hpp>

namespace Engine
{
class Texture2D : public Texture
{
public:
    Texture2D();
    Texture2D(const TextureSpecification &specification, Buffer data);
    Texture2D(const TextureSpecification &specification);
    Texture2D(ImageFormat format, int with = 1, int height = 1);
    ~Texture2D();

    void SetData(Buffer data) override;
    void Bind(uint32_t slot = 0) const override;
    void Unbind() const override;

    glm::vec2 GetSize() const override { return glm::vec2(m_Specification.Width, m_Specification.Height); }
    uint32_t GetWidth() const override { return m_Specification.Width; }
    uint32_t GetHeight() const override { return m_Specification.Height; }
    uint32_t GetRendererID() const override { return m_RendererID; }
    const TextureSpecification &GetSpecification() const override { return m_Specification; }

    static AssetType GetStaticType() { return AssetType::Texture2D; }
    AssetType GetType() const override { return GetStaticType(); }

    void Resize(glm::vec2 size) override;
    void AttachToFramebuffer(uint32_t attachment) override;

private:
    TextureSpecification m_Specification;
    unsigned int m_RendererID = 0;
    unsigned int m_InternalFormat, m_DataFormat, m_DataType;
};

using Texture2DRef = std::shared_ptr<Texture2D>;
} // namespace Engine
