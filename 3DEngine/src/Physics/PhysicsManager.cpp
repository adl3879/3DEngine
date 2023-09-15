#include "PhysicsManager.h"

#include "Scene.h"

namespace Engine
{
void PhysicsManager::RegisterBody(Physics::RigidBodyRef rb) { m_World->AddRigidBody(rb); }

void PhysicsManager::Step(float dt) 
{
    if (m_IsRunning || m_StepCount-- > 0) m_World->StepSimulation(dt);
}

void PhysicsManager::StepFrame(int steps) 
{
	m_StepCount = steps;
}

void PhysicsManager::Reset() { m_World->Clear(); }

void PhysicsManager::DrawDebug(Entity entity)
{
    if (m_DrawDebug) m_World->DrawDebug(entity);
}

void PhysicsManager::Init(Scene *scene)
{
    m_World = new Physics::DynamicWorld(scene);
    m_World->SetGravity(glm::vec3(0, -9.8, 0));

    m_IsRunning = false;
}
} // namespace Engine
