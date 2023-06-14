#pragma once

#include <entt.hpp>
#include <glm/glm.hpp>

namespace Engine
{
class Entity;
class Camera;

class Scene
{
  public:
    Scene() = default;
    virtual ~Scene();

    Entity CreateEntity(const std::string &name = std::string());

    void OnUpdate(float dt);
    void RenderScene();

  private:
    entt::registry m_Registry;

    friend class Entity;
    friend class SceneHierarchyPanel;
};
} // namespace Engine