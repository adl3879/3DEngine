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

    void SetContext(const std::shared_ptr<Scene> &context) { m_Context = context; }

    void OnImGuiRender();

  private:
    std::shared_ptr<Scene> m_Context;
};
} // namespace Engine
