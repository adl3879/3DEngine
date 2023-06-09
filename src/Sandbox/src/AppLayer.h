#pragma once

#include "Engine.h"
#include "SceneHierarchyPanel.h"

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
    void OnMouseScrolled(double xOffset, double yOffset) override;

  private:
    void NewScene();
    void OpenScene();
    void SaveSceneAs();
    void SaveScene();

  private:
    std::shared_ptr<Framebuffer> m_Framebuffer;

    EditorCamera m_EditorCamera;

    glm::vec2 m_ViewportSize;
    bool m_ViewportFocused = false;
    glm::vec2 m_ViewportBounds[2];

    // scene
    std::shared_ptr<Scene> m_Scene;
    Entity m_LightEntity;
    Entity m_LightEntity2;

    // panels
    SceneHierarchyPanel m_SceneHierarchyPanel;

  private:
    int m_GizmoType = -1;
};
} // namespace Engine