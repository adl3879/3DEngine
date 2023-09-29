#include "PhysicsSystem.h"

#include "Scene.h"
#include "Log.h"
#include "Components.h"
#include "PhysicsComponents.h"
#include "RigidBody.h"
#include "PhysicsManager.h"

namespace Engine
{
PhysicsSystem::PhysicsSystem(Scene *scene) { m_Scene = scene; }

bool PhysicsSystem::Init()
{
    LOG_CORE_INFO("Initializing Physics System");

    // We need to initialize shapes first, then bodies...
    InitializeShapes();

    InitializeRigidbodies();

    LOG_CORE_INFO("Physics System Initialized");
    return true;
}

void PhysicsSystem::Update(float dt)
{
    // ?
}

void PhysicsSystem::Draw() {}

void PhysicsSystem::FixedUpdate(float dt)
{
    InitializeRigidbodies();
    ApplyForces();

    PhysicsManager::Get().Step(dt);
}

void PhysicsSystem::Exit() {}

void PhysicsSystem::InitializeShapes()
{
    auto boxView = m_Scene->GetRegistry().view<BoxColliderComponent>();
    for (auto entity : boxView)
    {
        Entity ent = Entity{entity, m_Scene};
        auto &boxComponent = ent.GetComponent<BoxColliderComponent>();
        boxComponent.Box = std::make_shared<Physics::Box>(boxComponent.Size);
    }

    // TODO: Add support for other shapes
}

void PhysicsSystem::InitializeRigidbodies()
{
    // this code will be called if i have rigid body component
    auto view = m_Scene->GetRegistry().view<TransformComponent, RigidBodyComponent>();
    for (auto entity : view)
    {
        auto [transform, rb] = view.get<TransformComponent, RigidBodyComponent>(entity);
        Entity ent = Entity{entity, m_Scene};
        auto &rigidBodyComponent = ent.GetComponent<RigidBodyComponent>();
        Physics::RigidBodyRef rigidBody;

        if (rigidBodyComponent.GetRigidBody()) continue;

        // rigidBody will not be registered if it has no shape
        if (ent.HasComponent<BoxColliderComponent>())
        {
            float mass = rigidBodyComponent.Mass;

            auto &boxComponent = ent.GetComponent<BoxColliderComponent>();
            Physics::BoxRef boxShape = std::make_shared<Physics::Box>(boxComponent.Size * transform.Scale);
            rigidBody = std::make_shared<Physics::RigidBody>(mass, transform.Translation, transform.Rotation,
                                                             transform.GetTransform(), boxShape, ent);

            rigidBody->MotionType = rigidBodyComponent.MotionType;
            rigidBody->LinearDamping = rigidBodyComponent.LinearDamping;
            rigidBody->AngularDamping = rigidBodyComponent.AngularDamping;
            rigidBody->IsKinematic = rigidBodyComponent.IsKinematic;
            rigidBody->UseGravity = rigidBodyComponent.UseGravity;
            PhysicsManager::Get().RegisterBody(rigidBody);
        }

        rigidBodyComponent.RigidBody = rigidBody;
    }

    // TODO: Add support for other shapes
}

void PhysicsSystem::ApplyForces()
{
    auto view = m_Scene->GetRegistry().view<TransformComponent, RigidBodyComponent>();
    for (auto entity : view)
    {
        auto [transform, rb] = view.get<TransformComponent, RigidBodyComponent>(entity);
        Entity ent = Entity{entity, m_Scene};
        auto &rigidBodyComponent = ent.GetComponent<RigidBodyComponent>();
        Physics::RigidBodyRef rigidBody;

        // Not initialized yet.
        if (!rigidBodyComponent.GetRigidBody() || rigidBodyComponent.QueuedForce == glm::vec3() ||
            rigidBodyComponent.Mass == 0.0)
            continue;

        PhysicsManager::Get().GetWorld()->AddForceToRigidBody(ent, rigidBodyComponent.QueuedForce);

        rigidBodyComponent.QueuedForce = glm::vec3();
    }
}
} // namespace Engine
