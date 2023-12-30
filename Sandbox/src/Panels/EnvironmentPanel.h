#pragma once

#include <memory>
#include "Scene.h"
#include "Environment.h"

namespace Engine
{
class EnvironmentPanel
{
  public:
    EnvironmentPanel();
    virtual ~EnvironmentPanel() = default;

    void SetContext(const SceneRef &context) { m_Context = context; }

    void OnImGuiRender();

  private:
    SceneRef m_Context;
};
} // namespace Engine
