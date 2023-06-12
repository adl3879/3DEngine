#pragma once

#include "Engine.h"

#include <memory>

class AppLayer : public Engine::Layer
{
  public:
    AppLayer();
    virtual ~AppLayer() = default;

    virtual void OnAttach() override;
    virtual void OnDetach() override;
    virtual void OnUpdate(float deltaTime) override;
    virtual void OnImGuiRender() override;

    virtual void OnKeyPressed(Engine::InputKey key, bool isRepeat) override;
    virtual void OnMouseMoved(double xPos, double yPos, double xOffset, double yOffset) override;
    virtual void OnMouseButtonPressed(Engine::MouseButton button) override;
    virtual void OnWindowResize(int width, int height) override;

  private:
    std::unique_ptr<Engine::Shader> m_ModelShader;
    std::unique_ptr<Engine::Model> m_Model;
    std::shared_ptr<Engine::Framebuffer> m_Framebuffer;
    std::shared_ptr<Engine::Light> m_Light;

    Engine::Camera m_Camera{};
    Engine::CameraController m_CameraController{m_Camera, 0.05f, 2.5f};
    glm::vec2 m_ViewportSize;
    bool m_ViewportFocused = false;
};