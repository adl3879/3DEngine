#pragma once

#include <entt.hpp>
#include <glm/glm.hpp>
#include <memory>

#include "PerspectiveCamera.h"
#include "EditorCamera.h"

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
    Entity *GetEntity(const std::string &name);
    void DestroyEntity(Entity entity);

    void OnUpdateRuntime(float dt);
    void OnUpdateEditor(float dt, EditorCamera &camera);

    const std::shared_ptr<PerspectiveCamera> &GetMainCamera() const { return m_MainCamera; }

    void SetSceneFilePath(const std::string &filepath) { m_SceneFilePath = filepath; }
    const std::string &GetSceneFilePath() const { return m_SceneFilePath; }

  private:
    entt::registry m_Registry;

    friend class Entity;
    friend class SceneHierarchyPanel;
    friend class SceneSerializer;

  private:
    std::shared_ptr<PerspectiveCamera> m_MainCamera = nullptr;
    std::string m_SceneFilePath = std::string();
};
} // namespace Engine