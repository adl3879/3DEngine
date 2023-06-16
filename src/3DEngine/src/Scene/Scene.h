#pragma once

#include <entt.hpp>
#include <glm/glm.hpp>
#include <memory>

#include "Camera.h"

namespace Engine
{
class Entity;
class Camera;

class Scene
{
  public:
    Scene();
    virtual ~Scene();

    Entity CreateEntity(const std::string &name = std::string());
    void DestroyEntity(Entity entity);

    void OnUpdate(float dt);
    void RenderScene();

    const std::shared_ptr<Camera> &GetSceneCamera() const { return m_SceneCamera; }

  private:
    entt::registry m_Registry;

    friend class Entity;
    friend class SceneHierarchyPanel;

  private:
    std::shared_ptr<Camera> m_EditorCamera = nullptr;
    std::shared_ptr<Camera> m_SceneCamera = nullptr;
};
} // namespace Engine