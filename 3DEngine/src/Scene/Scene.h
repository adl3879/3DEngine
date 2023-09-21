#pragma once

#include <entt.hpp>
#include <glm/glm.hpp>
#include <memory>
#include <string_view>

#include "PerspectiveCamera.h"
#include "EditorCamera.h"
#include "UUID.h"
#include "Asset.h"
#include "Environment.h"
#include "Light.h"

#include "System.h"

namespace Engine
{
class Entity;
class Camera;
class SceneRenderer;

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
	void DestroyEntityRecursive(Entity entity);

    Entity DuplicateEntity(Entity entity);

	Entity GetEntityByUUID(UUID uuid);
    Entity FindEntityByName(std::string_view name);

	void OnRuntimeStart();
	void OnRuntimeStop();
	void StepRuntimeFrame(int step);

    void OnUpdateRuntime(float dt);
    void OnUpdateEditor(float dt, EditorCamera &camera);

	bool IsPlaying() const { return m_IsPlaying; }
	bool IsPaused() const { return m_IsPaused; }
	void SetPlaying(bool playing) { m_IsPlaying = playing; }
	void SetPaused(bool paused) { m_IsPaused = paused; }
	void SetSceneName(const std::string &name) { m_SceneName = name; }
	const std::string &GetSceneName() const { return m_SceneName; }

    const std::shared_ptr<PerspectiveCamera> &GetMainCamera() const { return m_MainCamera; }

    void SetSceneFilePath(const std::string &filepath) { m_SceneFilePath = filepath; }
    const std::string &GetSceneFilePath() const { return m_SceneFilePath; }

    void SetSelectedEntity(entt::entity entity) { m_SelectedEntity = entity; }
    const entt::entity &GetSelectedEntity() const { return m_SelectedEntity; }

    const entt::registry &GetRegistry() { return m_Registry; }

    EnvironmentRef GetEnvironment() { return m_Environment; }
    void SetEnvironment(EnvironmentRef environment) { m_Environment = environment; }

	void SetFramebuffer(FramebufferRef framebuffer) { m_Framebuffer = framebuffer; }

    LightRef GetLights() { return m_Lights; }

	void SetViewportSize(int x, int y) { m_ViewportSize = glm::vec2(x, y) ; }

  public:
    virtual AssetType GetType() const override { return AssetType::Scene; }

  public:
    static std::shared_ptr<Scene> Copy(std::shared_ptr<Scene> src);

  private:
	bool m_IsPlaying = false;
	bool m_IsPaused = false;
	std::string m_SceneName;

  private:
    entt::registry m_Registry;
    entt::entity m_SelectedEntity = entt::null;

	std::unordered_map<UUID, entt::entity> m_EntityMap;

    friend class Entity;
    friend class SceneHierarchyPanel;
    friend class SceneSerializer;

  private:
    std::shared_ptr<PerspectiveCamera> m_MainCamera;
    std::shared_ptr<EditorCamera> m_EditorCamera;
    std::shared_ptr<Light> m_Lights;

    std::string m_SceneFilePath = std::string();
    SceneRenderer *m_SceneRenderer;

	FramebufferRef m_Framebuffer;
    EnvironmentRef m_Environment;

    std::vector<SystemRef> m_Systems;

	glm::vec2 m_ViewportSize = glm::vec2(0.0f);
};

using SceneRef = std::shared_ptr<Scene>;
} // namespace Engine
