#pragma once

#include <map>
#include <vector>
#include <memory>

#include <Jolt/Jolt.h>
#include <glm/glm.hpp>

#include "PhysicsShapes.h"
#include "RigidBody.h"
#include "Model.h"

namespace JPH
{
class PhysicsSystem;
class JobSystemThreadPool;
class ContactListener;
class BodyInterface;
class Shape;
class Character;

template <class T> class Ref;
} // namespace JPH

namespace Engine
{
class BPLayerInterfaceImpl;
class MyContactListener;

class Scene;

namespace Physics
{
class DynamicWorld
{
  public:
    DynamicWorld(Scene *scene);
    virtual ~DynamicWorld() = default;

    void DrawDebug(Entity entity);

  public:
    void SetGravity(const glm::vec3 &gravity);

    void AddRigidBody(RigidBodyRef rb);
    void AddForceToRigidBody(Entity entity, const glm::vec3 &force);

    void StepSimulation(float dt);
    void Clear();

  private:
    JPH::Ref<JPH::Shape> GetJoltShape(const PhysicShapeRef shape);
    void SyncEntitiesTransforms();
    void SyncCharactersTransforms();

  private:
    uint32_t m_StepCount;

    std::shared_ptr<JPH::PhysicsSystem> m_JoltPhysicsSystem;
    JPH::JobSystemThreadPool *m_JoltJobSystem;
    JPH::BodyInterface *m_JoltBodyInterface;
    BPLayerInterfaceImpl *m_JoltBroadphaseLayerInterface;

    std::vector<uint32_t> m_RegisteredBodies;
    std::map<uint32_t, JPH::Character *> m_RegisteredCharacters;

  private:
    Scene *m_Scene;

  private:
    // debug models
    ModelRef m_DebugCube;
};
} // namespace Physics
} // namespace Engine