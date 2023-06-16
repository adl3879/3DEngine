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
    virtual void OnMouseMoved(double xPos, double yPos, double xOffset, double yOffset) override;
    virtual void OnMouseButtonPressed(MouseButton button) override;
    virtual void OnWindowResize(int width, int height) override;

  private:
    std::shared_ptr<Framebuffer> m_Framebuffer;
    std::shared_ptr<Light> m_Light;

    Camera m_Camera{};
    CameraController m_CameraController{m_Camera, 0.05f, 2.5f};
    glm::vec2 m_ViewportSize;
    bool m_ViewportFocused = false;

    // scene
    std::shared_ptr<Scene> m_Scene;
    Entity m_ModelEntity;
    Entity m_CameraEntity;

    // panels
    SceneHierarchyPanel m_SceneHierarchyPanel;
};
}