#pragma once

#include <filesystem>

#include "Entity.h"
#include "Prefab.h"

namespace Engine
{
class PrefabSerializer
{
  public:
    PrefabSerializer(PrefabRef &prefab): m_Prefab(prefab) {}

    void Serialize(const std::filesystem::path &path);
    bool Deserialize(const std::filesystem::path &path);

  private:
    PrefabRef m_Prefab;
};
} // namespace Engine
