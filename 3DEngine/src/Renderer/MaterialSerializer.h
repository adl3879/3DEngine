#pragma once

#include "Material.h"
#include "SceneSerializer.h"

namespace Engine
{
class MaterialSerializer
{
  public:
    explicit MaterialSerializer(MaterialRef material);

    bool Serialize(const std::filesystem::path &filepath);
    bool Deserialize(const std::filesystem::path &filepath);

  private:
    MaterialRef m_Material;
};
} // namespace Engine
