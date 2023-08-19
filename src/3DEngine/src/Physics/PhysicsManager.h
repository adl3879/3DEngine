#pragma once

#include <vector>
#include <glm/glm.hpp>

#include "DynamicWorld.h"
#include "RigidBody.h"
#include "Entity.h"

namespace Engine
{
class Scene;

class PhysicsManager
{
  public:
    static PhysicsManager &Get()
    {
        static PhysicsManager instance;
        return instance;
    }

    Physics::DynamicWorld *GetWorld() { return m_World; }

    PhysicsManager() {}

    void SetDrawDebug(bool value) { m_DrawDebug = value; }

    bool GetDrawDebug() { return m_DrawDebug; }

    void Init(Scene *scene);

    void Start() { m_IsRunning = true; }
    void Stop() { m_IsRunning = false; }
    void DrawDebug(Entity entity);
    bool IsRunning() { return m_IsRunning; }
    void Step(float dt);

    void Reset();

    // std::vector<RaycastResult> Raycast(const Vector3 &from, const Vector3 &to);

    void RegisterBody(Physics::RigidBodyRef rb);
    // void RegisterGhostBody(Ref<GhostObject> rb);
    // void RegisterCharacterController(Ref<Physics::CharacterController> c);

  private:
    Physics::DynamicWorld *m_World;
    bool m_IsRunning = false;

    bool m_DrawDebug = false;
};
} // namespace Engine