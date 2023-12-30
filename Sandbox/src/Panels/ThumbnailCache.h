#pragma once

#include "Texture2D.h"
#include "Project.h"

#include <queue>

namespace Engine
{

struct ThumbnailImage
{
    uint64_t Timestamp;
    Texture2DRef Image;
};

class ThumbnailCache
{
  public:
    ThumbnailCache() = default;

    void OnUpdate();
    Texture2DRef GetOrCreateThumbnail(const std::filesystem::path &path);

  private:
    std::map<std::filesystem::path, ThumbnailImage> m_CachedImages;

    struct ThumbnailInfo
    {
        std::filesystem::path AbsolutePath;
        std::filesystem::path AssetPath;
        uint64_t Timestamp;
    };
    std::queue<ThumbnailInfo> m_Queue;

    // TEMP (replace with Hazel::Serialization)
    std::filesystem::path m_ThumbnailCachePath;
};

using ThumbnailCacheRef = std::shared_ptr<ThumbnailCache>;
} // namespace Engine