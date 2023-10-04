#pragma once

#include "Texture2D.h"
#include "Framebuffer.h"

#include <glm/glm.hpp>
#include <unordered_map>
#include <filesystem>

namespace Engine
{
class ThumbnailManager
{
  public:
	ThumbnailManager();
	~ThumbnailManager() = default;

    static ThumbnailManager &Get();

  public:
    bool IsThumbnailLoaded(const std::filesystem::path &path) const;
    Texture2DRef GetThumbnail(const std::filesystem::path &path);
    void MarkThumbnailAsDirty(const std::filesystem::path &path);
    Texture2DRef GenerateThumbnail(const std::filesystem::path &path, Texture2DRef texture);

  private:
    std::unordered_map<std::filesystem::path, Texture2DRef> m_Thumbnails;

	FramebufferRef m_Framebuffer;
	const glm::vec2 m_ThumbnailSize = { 128, 128 };
};
} // namespace Engine
