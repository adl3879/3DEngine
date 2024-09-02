#pragma once

#include "Scene.h"

namespace Engine
{
class ConsolePanel
{
  public:
    ConsolePanel();
    virtual ~ConsolePanel() = default;

    void SetContext(const SceneRef &context) { m_Context = context; }
    void OnImGuiRender();

  private:
    SceneRef m_Context;
};
} // namespace Engine
