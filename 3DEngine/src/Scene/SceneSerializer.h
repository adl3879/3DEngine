#pragma once

#include <memory>
#include <yaml-cpp/yaml.h>
#include <string>

#include "Scene.h"

namespace Engine
{
class SceneSerializer
{
  public:
    explicit SceneSerializer(const SceneRef &scene) : m_Scene(scene) {}

    void Serialize(const std::string &filepath);
    void SerializeRuntime(const std::string &filepath);

    bool Deserialize(const std::string &filepath);
    bool DeserializeRuntime(const std::string &filepath);

  public:
    static void SerializeEntity(YAML::Emitter &out, Entity entity);
    static void DeserializeEntity(YAML::detail::iterator_value entity, Entity deserializedEntity);

  private:
    std::shared_ptr<Scene> m_Scene;
};
} // namespace Engine
