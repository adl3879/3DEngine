#pragma once

#include "Engine.h"
#include "Panels/SceneHierarchyPanel.h"
#include "Panels/ContentBrowserPanel.h"
#include "Panels/MaterialEditorPanel.h"
#include "Panels/EnvironmentPanel.h"
#include "Project.h"
#include "Texture.h"
#include "Framebuffer.h"

#include <memory>

namespace Engine
{
class AppLayer : public Layer
{
  public:
    AppLayer();
    virtual ~AppLayer() = default;

    virtual void OnAttach() override;
    virtual void OnDetach() override;
    virtual void OnUpdate(float dt) override;
    virtual void OnFixedUpdate(float dt) override;
    virtual void OnImGuiRender() override;

    virtual void OnKeyPressed(InputKey key, bool isRepeat) override;
    virtual void OnMouseScrolled(double xOffset, double yOffset) override;
    virtual void OnMouseButtonPressed(MouseButton button) override;

  private:
    // Project
    void NewProject();
    void OpenProject();

    // New
    void NewScene();
    void OpenScene();
    void SaveSceneAs();
    void SaveScene();
    void ResetScene(const std::string &path);

    void DuplicateEntity();

    void OnScenePlay();
    void OnSceneStop();

    // UI Panels
    void UI_Toolbar();

  private:
    void DrawTransformControls(const char *icon, int type = -1);
	bool m_IsControlPressed = false;

  private:
    std::shared_ptr<Framebuffer> m_Framebuffer;

    EditorCamera m_EditorCamera;

    glm::vec2 m_ViewportSize;
    bool m_ViewportFocused, m_ViewportHovered = false;
    glm::vec2 m_ViewportBounds[2];

    // scene
    SceneRef m_ActiveScene, m_EditorScene;

    // panels
    SceneHierarchyPanel m_SceneHierarchyPanel;
    MaterialEditorPanel m_MaterialEditorPanel;
    EnvironmentPanel m_EnvironmentPanel;
    std::shared_ptr<ContentBrowserPanel> m_ContentBrowserPanel = nullptr;

  private:
    int m_GizmoType = -1;

    enum class SceneState
    {
        Edit = 0,
        Play,
        Simulate,
    };
    SceneState m_SceneState = SceneState::Edit;

    Texture2DRef m_IconPlay, m_IconStop, m_IconPause, m_IconSimulate;
};
} // namespace Engine
