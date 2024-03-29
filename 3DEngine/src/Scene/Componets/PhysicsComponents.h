#pragma once

#include <memory>

#include <glm/glm.hpp>

#include "Components.h"
#include "PhysicsShapes.h"

namespace Engine
{
namespace Physics
{
class RigidBody;
};

struct RigidBodyComponent
{
    Physics::RigidBodyRef RigidBody;

    float Mass = 1.0f;
    glm::vec3 QueuedForce;

    Physics::MotionType MotionType = Physics::MotionType::Static;
    float LinearDamping = 0.05f;
    float AngularDamping = 0.05f;
    bool IsKinematic = false;
    bool UseGravity = true;

    RigidBodyComponent() = default;
    RigidBodyComponent(const RigidBodyComponent &) = default;

    std::shared_ptr<Physics::RigidBody> GetRigidBody() const { return RigidBody; }

    void SyncTransformComponent(TransformComponent *tc){};
    void SyncWithTransform(TransformComponent *tc){};
};

struct BoxColliderComponent
{
    Physics::PhysicShapeRef Box;
    glm::vec3 Size = glm::vec3(0.5f, 0.5f, 0.5f);
    bool IsTrigger = false;

    BoxColliderComponent() = default;
    BoxColliderComponent(const BoxColliderComponent &) = default;
};
} // namespace Engine