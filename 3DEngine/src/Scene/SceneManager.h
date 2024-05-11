#pragma once

#include "Scene.h"

namespace Engine
{
enum class SceneState
{
    Edit = 0,
    Play
};

class SceneManager
{
  public:
    static SceneManager &Get();

    SceneManager();
    virtual ~SceneManager() = default;

	void SetActiveScene(const SceneRef &scene);
	SceneRef GetActiveScene() const { return m_ActiveScene; }

	void SetEditorScene(const SceneRef &scene) { m_EditorScene = scene; }
	SceneRef GetEditorScene() const { return m_EditorScene; }

	SceneState GetSceneState() const { return m_SceneState; }
	void SetSceneState(SceneState sceneState) { m_SceneState = sceneState; }

	void ShowMainScene();

    void NewScene();
    void OpenScene();
    void SaveSceneAs();
    void SaveScene();

    void OnScenePlay();
    void OnSceneStop();

	private:
    SceneRef m_ActiveScene, m_EditorScene;

    SceneState m_SceneState = SceneState::Edit;
};
} // namespace Engine
