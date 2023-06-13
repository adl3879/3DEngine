#pragma once

#include <entt.hpp>

namespace Engine
{
class Entity;

class Scene
{
  public:
    Scene();
    virtual ~Scene();

    Entity CreateEntity(const std::string &name = std::string());
    void OnUpdate(float dt);

  private:
    entt::registry m_Registry;

    friend class Entity;
};
} // namespace Engine