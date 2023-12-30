#pragma once

#include "Engine.h"
#include <memory>

namespace Engine
{
class SceneHierarchyPanel
{
  public:
    SceneHierarchyPanel() = default;
    SceneHierarchyPanel(const std::shared_ptr<Scene> &context);
    ~SceneHierarchyPanel() = default;

    Entity CreateEntityPopup();

    void SetContext(const std::shared_ptr<Scene> &context);
    Entity GetSelectedEntity() const { return m_SelectionContext; }
    void SetSelectedEntity(Entity entity);

    void OnImGuiRender();

  private:
    void DrawEntityNode(Entity entity);
    void DrawComponents(Entity entity);

    SceneRef m_Context;
    Entity m_SelectionContext;
};
} // namespace Engine
