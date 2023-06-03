#pragma once

#include <imgui.h>
#include <backends/imgui_impl_opengl3.h>
#include <backends/imgui_impl_glfw.h>

#include "Layer.h"
#include "InputManager.h"

namespace Engine
{
class ImGuiLayer : public Layer
{
  public:
    ImGuiLayer();
    virtual ~ImGuiLayer();

    virtual void OnAttach() override;
    virtual void OnDetach() override;
    virtual void OnUpdate(float deltaTime) override;

    virtual void OnKeyPressed(InputKey key, bool isRepeat) override;
    virtual void OnKeyReleased(InputKey key) override;
    virtual void OnMouseButtonPressed(MouseButton button) override;
    virtual void OnMouseButtonReleased(MouseButton button) override;
    virtual void OnMouseMoved(double xPos, double yPos, double xOffset, double yOffset) override;
    virtual void OnMouseScrolled(double xOffset, double yOffset) override;
    virtual void OnWindowResize(int width, int height) override;

    void Begin();
    void End();

  private:
    float m_Time = 0.0f;
};
} // namespace Engine