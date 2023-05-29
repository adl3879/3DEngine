#pragma once

#include <string>
#include "InputKey.h"

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
    virtual void OnMouseButtonPressed(InputKey key) {}
    virtual void OnMouseMoved(double xPos, double yPos) {}

  protected:
    std::string m_DebugName;
};
