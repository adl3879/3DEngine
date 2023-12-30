#pragma once

#include <memory>
#include <glm/glm.hpp>

namespace Engine
{
class Scene;

class System
{
public:
    virtual bool Init() { return true; }

    virtual void Draw(const glm::mat4 &projection, const glm::mat4 &view){}

    virtual void Update(float dt){}
    virtual void FixedUpdate(float dt){}
    virtual void EditorUpdate(){}
    virtual void Exit(){}

    virtual void OnRuntimeStart(){}
    virtual void OnRuntimeStop(){}
    virtual void OnRuntimeUpdate(float dt){}


public:
    Scene *m_Scene;
};

using SystemRef = std::shared_ptr<System>;
} // namespace Engine
