#pragma once

#include "Texture2D.h"
#include "Framebuffer.h"
#include "Asset.h"
#include "Material.h"
#include "Model.h"

#include <glm/glm.hpp>
#include <unordered_map>
#include <filesystem>
#include <queue>

namespace Engine
{
class ThumbnailManager
{
  public:
    ThumbnailManager();
    ~ThumbnailManager() = default;

    void OnUpdate();

    static ThumbnailManager &Get();

  public:
    Texture2DRef GetThumbnail(std::filesystem::path path);

    [[nodiscard]] bool IsThumbnailLoaded(std::filesystem::path path) const;
    void MarkThumbnailAsDirty(AssetHandle handle);

  private:
    void GenerateMaterialThumbnail(MaterialRef material, Texture2DRef texture);

  private:
    std::unordered_map<std::filesystem::path, Texture2DRef> m_Thumbnails;

    std::queue<std::filesystem::path> m_Queue;

    FramebufferRef m_Framebuffer;
    const glm::ivec2 m_ThumbnailSize = { 256, 256 };
};
} // namespace Engine
