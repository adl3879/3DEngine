#pragma once

#include "Engine.h"
#include "SceneHierarchyPanel.h"
#include "RenderSystem.h"
#include "ContentBrowserPanel.h"
#include "Project.h"

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
    virtual void OnUpdate(float deltaTime) override;
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

    void OnScenePlay();
    void OnSceneStop();

  private:
    void FileOperations();

    // UI Panels
    void UI_Toolbar();

  private:
    std::shared_ptr<Framebuffer> m_Framebuffer;
    std::shared_ptr<Framebuffer> m_Framebuffer2;

    EditorCamera m_EditorCamera;

    glm::vec2 m_ViewportSize;
    bool m_ViewportFocused, m_ViewportHovered = false;
    glm::vec2 m_ViewportBounds[2];

    // scene
    std::shared_ptr<Scene> m_Scene;

    // panels
    SceneHierarchyPanel m_SceneHierarchyPanel;
    ContentBrowserPanel m_ContentBrowserPanel;

    RenderSystemPtr m_RenderSystem;

  private:
    int m_GizmoType = -1;
    Entity m_HoveredEntity;

    enum class SceneState
    {
        Edit = 0,
        Play = 1
    };
    SceneState m_SceneState = SceneState::Edit;

    unsigned int m_IconPlay, m_IconStop;
};
} // namespace Engine