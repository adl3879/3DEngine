#pragma once

#include <memory>
#include <string>

#include "Scene.h"

namespace Engine
{
class SceneSerializer
{
  public:
    SceneSerializer(const std::shared_ptr<Scene> &scene) : m_Scene(scene) {}

    void Serialize(const std::string &filepath);
    void SerializeRuntime(const std::string &filepath);

    bool Deserialize(const std::string &filepath);
    bool DeserializeRuntime(const std::string &filepath);

  private:
    std::shared_ptr<Scene> m_Scene;
};
} // namespace Engine
