#pragma once

#include <string>
#include "InputKey.h"

namespace Engine
{
class Layer
{
  public:
    Layer(const std::string &name = "Layer");
    virtual ~Layer() = default;

    virtual void OnAttach() {}
    virtual void OnDetach() {}
    virtual void OnUpdate(float deltaTime) {}
    virtual void OnImGuiRender() {}

    virtual void OnKeyPressed(InputKey key, bool isRepeat) {}
    virtual void OnKeyReleased(InputKey key) {}
    virtual void OnMouseButtonPressed(MouseButton button) {}
    virtual void OnMouseButtonReleased(MouseButton button) {}
    virtual void OnMouseMoved(double xPos, double yPos, double xOffset, double yOffset) {}
    virtual void OnMouseScrolled(double xOffset, double yOffset) {}
    virtual void OnWindowResize(int width, int height) {}

  protected:
    std::string m_DebugName;
};
} // namespace Engine