#pragma once

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

    void Begin();
    void End();

  private:
    float m_Time = 0.0f;
};
} // namespace Engine