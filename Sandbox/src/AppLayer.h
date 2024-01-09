#pragma once

#include "Panels/SceneHierarchyPanel.h"
#include "Panels/ContentBrowserPanel.h"
#include "Panels/MaterialEditorPanel.h"
#include "Panels/EnvironmentPanel.h"
#include "Panels/InputMapPanel.h"
#include "Framebuffer.h"

#include <memory>
#include <functional>

namespace Engine
{
class AppLayer : public Layer
{
  public:
    AppLayer();
    ~AppLayer() override = default;

    void OnAttach() override;
    void OnDetach() override;
    void OnUpdate(float dt) override;
    void OnFixedUpdate(float dt) override;
    void OnImGuiRender() override;

    void OnKeyPressed(InputKey key, bool isRepeat) override;
    void OnMouseScrolled(double xOffset, double yOffset) override;
    void OnMouseButtonPressed(MouseButton button) override;

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
    void DrawControls(const char *icon, const char *tooltip, bool isActive, std::function<void()> action);
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
    std::unique_ptr<ContentBrowserPanel> m_ContentBrowserPanel;
    InputMapPanel m_InputMapPanel;

  private:
    int m_GizmoType = -1;

    enum class SceneState
    {
        Edit = 0,
        Play,
    };

    SceneState m_SceneState = SceneState::Edit;

    Texture2DRef m_IconPlay, m_IconStop, m_IconPause, m_IconSimulate;
};
} // namespace Engine
