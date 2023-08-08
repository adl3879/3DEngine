#pragma once

#include "Material.h"
#include "SceneSerializer.h"

#include <yaml-cpp/yaml.h>

namespace Engine
{
class MaterialSerializer
{
  public:
    MaterialSerializer(MaterialRef material);

    bool Serialize(const std::filesystem::path &filepath);
    bool Deserialize(const std::filesystem::path &filepath);

  private:
    MaterialRef m_Material;
};
} // namespace Engine