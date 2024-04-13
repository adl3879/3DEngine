#pragma once

#include "Texture.h"

#include <glm/glm.hpp>

namespace Engine
{
class Texture2DArray : public Texture
{
  public:
    Texture2DArray();
    Texture2DArray(ImageFormat format, int width = 1, int height = 1);
    ~Texture2DArray();

    void SetData(Buffer data) override;
    void Bind(uint32_t slot = 0) const override;
    void Unbind() const override;

    glm::vec2 GetSize() const override { return glm::vec2(m_Specification.Width, m_Specification.Height); }
    uint32_t GetWidth() const override { return m_Specification.Width; }
    uint32_t GetHeight() const override { return m_Specification.Height; }
    uint32_t GetRendererID() const override { return m_RendererID; }
    const TextureSpecification &GetSpecification() const override { return m_Specification; }

    void Resize(glm::vec2 size) override;
    void AttachToFramebuffer(uint32_t attachment) override;

	static AssetType GetStaticType() { return AssetType::Texture2D; }
    AssetType GetType() const override { return GetStaticType(); }

  private:
    TextureSpecification m_Specification;
    unsigned int m_RendererID = 0;
    unsigned int m_InternalFormat, m_DataFormat, m_DataType;
};

using Texture2DArrayRef = std::shared_ptr<Texture2DArray>;
} // namespace Engine
