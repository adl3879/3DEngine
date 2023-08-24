#pragma once

#include <entt.hpp>
#include <glm/glm.hpp>
#include <memory>

#include "PerspectiveCamera.h"
#include "EditorCamera.h"
#include "UUID.h"
#include "Asset.h"

#include "System.h"

namespace Engine
{
class Entity;
class Camera;

class Scene : public Asset
{
  public:
    Scene();
    virtual ~Scene();

    void OnAttach();
    void OnDetach();

    void OnUpdate(float dt);
    void OnFixedUpdate(float dt);

    Entity CreateEntity(const std::string &name = std::string());
    Entity CreateEntityWithUUID(UUID uuid, const std::string &name = std::string());
    Entity *GetEntity(const std::string &name);
    void DestroyEntity(Entity entity);
    Entity DuplicateEntity(Entity entity);

    void OnUpdateRuntime(float dt);
    void OnUpdateEditor(float dt, EditorCamera &camera);

    const std::shared_ptr<PerspectiveCamera> &GetMainCamera() const { return m_MainCamera; }

    void SetSceneFilePath(const std::string &filepath) { m_SceneFilePath = filepath; }
    const std::string &GetSceneFilePath() const { return m_SceneFilePath; }

    void SetSelectedEntity(entt::entity entity) { m_SelectedEntity = entity; }
    const entt::entity &GetSelectedEntity() const { return m_SelectedEntity; }

    const entt::registry &GetRegistry() { return m_Registry; }

  public:
    virtual AssetType GetType() const override { return AssetType::Scene; }

  public:
    static std::shared_ptr<Scene> Copy(std::shared_ptr<Scene> src);

  private:
    entt::registry m_Registry;
    entt::entity m_SelectedEntity = entt::null;

    friend class Entity;
    friend class SceneHierarchyPanel;
    friend class SceneSerializer;

  private:
    std::shared_ptr<PerspectiveCamera> m_MainCamera = nullptr;
    std::string m_SceneFilePath = std::string();

    std::vector<SystemRef> m_Systems;
};

using SceneRef = std::shared_ptr<Scene>;
} // namespace Engine