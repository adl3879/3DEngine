#pragma once

#include <memory>

namespace Engine
{
class Scene;

class System
{
  public:
    virtual bool Init() { return true; }

    virtual void Draw() {}

    virtual void Update(float dt) {}
    virtual void FixedUpdate(float dt) {}
    virtual void EditorUpdate() {}
    virtual void Exit() {}

  public:
    Scene *m_Scene;
};

using SystemRef = std::shared_ptr<System>;
} // namespace Engine
