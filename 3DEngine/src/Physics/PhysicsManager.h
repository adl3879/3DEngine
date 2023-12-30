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

    [[nodiscard]] Physics::DynamicWorld *GetWorld() const { return m_World; }

    void SetDrawDebug(bool value) { m_DrawDebug = value; }
    [[nodiscard]] bool GetDrawDebug() const { return m_DrawDebug; }

    void Init(Scene *scene);

    void Start() { m_IsRunning = true; }
    void Stop() { m_IsRunning = false; }
    void DrawDebug(const glm::mat4 &projection, const glm::mat4 &view, Entity entity);
    [[nodiscard]] bool IsRunning() const { return m_IsRunning; }
    void Step(float dt);
    void StepFrame(int steps);

    void Reset();

    // std::vector<RaycastResult> Raycast(const Vector3 &from, const Vector3 &to);

    void RegisterBody(Physics::RigidBodyRef rb);
    // void RegisterGhostBody(Ref<GhostObject> rb);
    // void RegisterCharacterController(Ref<Physics::CharacterController> c);

private:
    PhysicsManager() = default;

private:
    Physics::DynamicWorld *m_World = nullptr;
    bool m_IsRunning = false;
    int m_StepCount = 0;

    bool m_DrawDebug = true;
};
} // namespace Engine
