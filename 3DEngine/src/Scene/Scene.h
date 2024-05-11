#pragma once

#include <entt.hpp>
#include <memory>

#include "PerspectiveCamera.h"
#include "EditorCamera.h"
#include "UUID.h"
#include "Asset.h"
#include "Environment.h"
#include "Light.h"
#include "ImGuiTextEditor.h"

#include "System.h"

namespace Engine
{
class Entity;
class Camera;
class SceneRenderer;

enum class SceneType
{
	None = 0,
	Scene2D,
	Scene3D,
	Prefab2D,
	Prefab3D,
};

class Scene : public Asset
{
public:
    Scene(SceneType sceneType = SceneType::Scene3D);
	explicit Scene(const std::string &name, SceneType sceneType = SceneType::Scene3D);
    virtual ~Scene();

    void OnAttach();
    void OnDetach();

    void OnUpdate(float dt);
    void OnFixedUpdate(float dt);

    Entity CreateEntity(const std::string &name = std::string());
    Entity CreateEntityWithUUID(UUID uuid, const std::string &name = std::string());
    Entity *GetEntity(const std::string &name);
	void AddToRootEntity(Entity &entity);
	void DestroyRootEntity();
	void SetRootEntity(Entity entity);
	UUID GetRootEntityUUID() const { return m_RootEntity; }

    void DestroyEntity(Entity entity);
    void DestroyEntityRecursive(Entity entity);
    Entity DuplicateEntity(Entity entity);
    Entity DuplicateEntityRecursive(Entity entity, Entity parent);
	void ReplaceEntity(Entity oldEntity, Entity newEntity);
	Entity GetPrefabRoot(Entity entity);
    
	Entity GetEntityByUUID(UUID uuid);
    Entity FindEntityByName(std::string_view name);

    void OnRuntimeStart();
    void OnRuntimeStop();
    void OnRuntimeUpdate(float dt);
    void StepRuntimeFrame(int step);
    void OnUpdateEditor(float dt, EditorCamera &camera);

    void GenerateNETSolution();

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

    void SetViewportSize(int x, int y) { m_ViewportSize = glm::vec2(x, y); }
    void SetViewportMousePos(int x, int y) { m_ViewportMousePos = glm::ivec2(x, y); }

    void SetHoveredEntity(entt::entity entity) { m_HoveredEntity = entity; }
    entt::entity GetHoveredEntity() const { return m_HoveredEntity; }

    glm::vec2 GetViewportMousePos() const { return m_ViewportMousePos; }

    void SetDebugDraw(bool debugDraw) { m_IsDebugDraw = debugDraw; }
    bool IsDebugDrawEnabled() const { return m_IsDebugDraw; }

    void SetGridEnabled(bool gridEnabled) { m_IsGridEnabled = gridEnabled; }
    bool IsGridEnabled() const { return m_IsGridEnabled; }

    void InitTextEditor(const std::filesystem::path &filepath);
    void RenderTextEditors();
    bool IsTextEditorFocused();

    AssetType GetType() const override { return AssetType::Scene; }

    static std::shared_ptr<Scene> Copy(std::shared_ptr<Scene> src);

	void UseCustomCamera(PerspectiveCamera *camera) { m_TempCamera = camera; }
    void UnuseCustomCamera() { m_TempCamera = nullptr; }
	Camera &GetCamera();
    auto GetEditorCamera() { return m_EditorCamera; }

	SceneType GetSceneType() const { return m_SceneType; }
    void SetSceneType(SceneType type) { m_SceneType = type; }
	void SetCurrentPrefabScene(UUID uuid) { m_CurrentPrefabScene = uuid; }
    UUID GetCurrentPrefabScene() const {return m_CurrentPrefabScene; }

private:
    void New(const std::string &name);

    bool m_IsPlaying = false;
    bool m_IsPaused = false;
    std::string m_SceneName;

    entt::registry m_Registry;
    entt::entity m_SelectedEntity = entt::null;
    entt::entity m_HoveredEntity = entt::null;
    UUID m_RootEntity;

    std::unordered_map<UUID, entt::entity> m_EntityMap;

    friend class Entity;
    friend class SceneHierarchyPanel;
    friend class SceneSerializer;
	friend class PropertiesPanel;

    std::shared_ptr<PerspectiveCamera> m_MainCamera;
    std::shared_ptr<EditorCamera> m_EditorCamera;
    PerspectiveCamera *m_TempCamera = nullptr;

    std::shared_ptr<Light> m_Lights;
    std::unordered_map<std::filesystem::path, ImGuiTextEditorRef> m_TextEditors;
    bool m_IsDebugDraw = false;
    bool m_IsGridEnabled = true;

    std::string m_SceneFilePath = std::string();
    SceneRenderer *m_SceneRenderer;

    FramebufferRef m_Framebuffer;
    EnvironmentRef m_Environment;

    std::vector<SystemRef> m_Systems;

    glm::vec2 m_ViewportSize = glm::vec2(0.0f);
    glm::ivec2 m_ViewportMousePos;

	SceneType m_SceneType = SceneType::Scene3D;
    UUID m_CurrentPrefabScene;
};

using SceneRef = std::shared_ptr<Scene>;
} // namespace Engine
