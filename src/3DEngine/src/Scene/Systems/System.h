#pragma once

#include <memory>

namespace Engine
{
class Scene;

class System
{
  public:
    virtual bool Init() = 0;

    virtual void Draw() = 0;

    virtual void Update(float dt) = 0;
    virtual void FixedUpdate(float dt) = 0;
    virtual void EditorUpdate() {}
    virtual void Exit() = 0;

  public:
    Scene *m_Scene;
};

using SystemRef = std::shared_ptr<System>;
} // namespace Engine