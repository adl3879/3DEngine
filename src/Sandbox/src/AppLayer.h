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

    virtual void OnKeyPressed(Engine::InputKey key, bool isRepeat) override;
    virtual void OnMouseMoved(double xPos, double yPos) override;
    virtual void OnMouseButtonPressed(Engine::MouseButton button) override;

  private:
    std::unique_ptr<Engine::Shader> m_ModelShader;
    std::unique_ptr<Engine::Model> m_Model;

    Engine::Camera m_Camera{};
    Engine::CameraController m_CameraController{m_Camera, 0.1f, 2.5f};
};