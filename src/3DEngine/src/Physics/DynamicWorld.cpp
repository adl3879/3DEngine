#include "DynamicWorld.h"

#include "Log.h"

#include <Jolt/Jolt.h>
#include <Jolt/RegisterTypes.h>
#include <Jolt/Core/Factory.h>
#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Core/JobSystemThreadPool.h>
#include <Jolt/Physics/PhysicsSettings.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>
#include <Jolt/Physics/Collision/Shape/CapsuleShape.h>
#include <Jolt/Physics/Collision/Shape/CylinderShape.h>
#include <Jolt/Physics/Collision/Shape/MeshShape.h>
#include <Jolt/Physics/Collision/Shape/ConvexHullShape.h>

#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Body/BodyActivationListener.h>
#include <Jolt/Physics/Character/Character.h>
#include <Jolt/Physics/Collision/RayCast.h>
#include <Jolt/Physics/Collision/CastResult.h>

#include <glm/ext/quaternion_common.hpp>
#include <glm/gtx/matrix_decompose.hpp>

#include "Components.h"
#include "PhysicsComponents.h"
#include "Scene.h"
#include "MeshImporter.h"

#include <iostream>

namespace Engine
{
using namespace JPH;

// Callback for traces, connect this to your own trace function if you have one
static void TraceImpl(const char *inFMT, ...)
{
    // Format the message
    va_list list;
    va_start(list, inFMT);
    char buffer[1024];
    vsnprintf(buffer, sizeof(buffer), inFMT, list);

    // Print to the TTY
    std::cout << buffer << std::endl;
}

#ifdef JPH_ENABLE_ASSERTS

// Callback for asserts, connect this to your own assert handler if you have one
static bool AssertFailedImpl(const char *inExpression, const char *inMessage, const char *inFile, uint32_t inLine)
{
    // Print to the TTY
    std::cout << inFile << ":" << inLine << ": (" << inExpression << ") " << (inMessage != nullptr ? inMessage : "")
              << std::endl;

    // Breakpoint
    return true;
};

#endif // JPH_ENABLE_ASSERTS

// Layer that objects can be in, determines which other objects it can collide with
// Typically you at least want to have 1 layer for moving bodies and 1 layer for static bodies, but you can have more
// layers if you want. E.g. you could have a layer for high detail collision (which is not used by the physics
// simulation but only if you do collision testing).
namespace Layers
{
static constexpr uint8_t NON_MOVING = 0;
static constexpr uint8_t MOVING = 1;
static constexpr uint8_t NUM_LAYERS = 2;
}; // namespace Layers

// Function that determines if two object layers can collide
static bool MyObjectCanCollide(JPH::ObjectLayer inObject1, JPH::ObjectLayer inObject2)
{
    switch (inObject1)
    {
        case Layers::NON_MOVING: return inObject2 == Layers::MOVING; // Non moving only collides with moving
        case Layers::MOVING: return true;                            // Moving collides with everything
        default:
            // JPH_ASSERT(false);
            return false;
    }
};

// Each broadphase layer results in a separate bounding volume tree in the broad phase. You at least want to have
// a layer for non-moving and moving objects to avoid having to update a tree full of static objects every frame.
// You can have a 1-on-1 mapping between object layers and broadphase layers (like in this case) but if you have
// many object layers you'll be creating many broad phase trees, which is not efficient. If you want to fine tune
// your broadphase layers define JPH_TRACK_BROADPHASE_STATS and look at the stats reported on the TTY.
namespace BroadPhaseLayers
{
static constexpr JPH::BroadPhaseLayer NON_MOVING(0);
static constexpr JPH::BroadPhaseLayer MOVING(1);
static constexpr uint32_t NUM_LAYERS(2);
}; // namespace BroadPhaseLayers

// BroadPhaseLayerInterface implementation
// This defines a mapping between object and broadphase layers.
class BPLayerInterfaceImpl final : public JPH::BroadPhaseLayerInterface
{
  public:
    BPLayerInterfaceImpl()
    {
        // Create a mapping table from object to broad phase layer
        mObjectToBroadPhase[Layers::NON_MOVING] = BroadPhaseLayers::NON_MOVING;
        mObjectToBroadPhase[Layers::MOVING] = BroadPhaseLayers::MOVING;
    }

    virtual JPH::uint GetNumBroadPhaseLayers() const override { return BroadPhaseLayers::NUM_LAYERS; }

    virtual JPH::BroadPhaseLayer GetBroadPhaseLayer(JPH::ObjectLayer inLayer) const override
    {
        using namespace JPH;
        JPH_ASSERT(inLayer < Layers::NUM_LAYERS);
        return mObjectToBroadPhase[inLayer];
    }

#if defined(JPH_EXTERNAL_PROFILE) || defined(JPH_PROFILE_ENABLED)
    virtual const char *GetBroadPhaseLayerName(BroadPhaseLayer inLayer) const override
    {
        switch ((BroadPhaseLayer::Type)inLayer)
        {
            case (BroadPhaseLayer::Type)BroadPhaseLayers::NON_MOVING: return "NON_MOVING";
            case (BroadPhaseLayer::Type)BroadPhaseLayers::MOVING: return "MOVING";
            default: JPH_ASSERT(false); return "INVALID";
        }
    }
#endif // JPH_EXTERNAL_PROFILE || JPH_PROFILE_ENABLED

  private:
    JPH::BroadPhaseLayer mObjectToBroadPhase[Layers::NUM_LAYERS];
};

// Function that determines if two broadphase layers can collide
static bool MyBroadPhaseCanCollide(JPH::ObjectLayer inLayer1, JPH::BroadPhaseLayer inLayer2)
{
    using namespace JPH;
    switch (inLayer1)
    {
        case Layers::NON_MOVING: return inLayer2 == BroadPhaseLayers::MOVING;
        case Layers::MOVING: return true;
        default: JPH_ASSERT(false); return false;
    }
}

class ObjectVsBroadPhaseLayerFilterImpl : public JPH::ObjectVsBroadPhaseLayerFilter
{
  public:
    virtual bool ShouldCollide(JPH::ObjectLayer inLayer1, JPH::BroadPhaseLayer inLayer2) const override
    {
        switch (inLayer1)
        {
            case Layers::NON_MOVING: return inLayer2 == BroadPhaseLayers::MOVING;
            case Layers::MOVING: return true;
            default: return false;
        }
    }
};

class ObjectLayerPairFilterImpl : public JPH::ObjectLayerPairFilter
{
  public:
    virtual bool ShouldCollide(JPH::ObjectLayer inObject1, JPH::ObjectLayer inObject2) const override
    {
        switch (inObject1)
        {
            case Layers::NON_MOVING: return inObject2 == Layers::MOVING; // Non moving only collides with moving
            case Layers::MOVING: return true;                            // Moving collides with everything
            default: return false;
        }
    }
};

BPLayerInterfaceImpl JoltBroadphaseLayerInterface;
ObjectVsBroadPhaseLayerFilterImpl JoltObjectVSBroadphaseLayerFilter;
ObjectLayerPairFilterImpl JoltObjectVSObjectLayerFilter;

namespace Physics
{
DynamicWorld::DynamicWorld(Scene *scene) : m_StepCount(0), m_Scene(scene)
{
    // Register allocation hook
    RegisterDefaultAllocator();

    // Install callbacks
    Trace = TraceImpl;
    JPH_IF_ENABLE_ASSERTS(AssertFailed = AssertFailedImpl;)

    // Create a factory
    Factory::sInstance = new Factory();

    // Register all Jolt physics types
    RegisterTypes();

    m_RegisteredCharacters = std::map<uint32_t, JPH::Character *>();

    // initialize jolt physics
    const uint32_t MaxBodies = 2048;
    const uint32_t NumBodyMutexes = 0;
    const uint32_t MaxBodyPairs = 1024;
    const uint32_t MaxContactConstraints = 1024;

    m_JoltPhysicsSystem = std::make_shared<JPH::PhysicsSystem>();
    m_JoltPhysicsSystem->Init(MaxBodies, NumBodyMutexes, MaxBodyPairs, MaxContactConstraints,
                              JoltBroadphaseLayerInterface, JoltObjectVSBroadphaseLayerFilter,
                              JoltObjectVSObjectLayerFilter);

    // m_ContactListener = std::make_unique<MyContactListener>();
    // m_JoltPhysicsSystem->SetContactListener(m_ContactListener.get());

    // The main way to interact with the bodies in the physics system is through the body interface.There is a locking
    // and a non - locking variant of this. We're going to use the locking version (even though we're not planning to
    // access bodies from multiple threads)
    m_JoltBodyInterface = &m_JoltPhysicsSystem->GetBodyInterface();

    // Optional step: Before starting the physics simulation you can optimize the broad phase. This improves collision
    // detection performance (it's pointless here because we only have 2 bodies). You should definitely not call this
    // every frame or when e.g. streaming in a new level section as it is an expensive operation. Instead insert all new
    // objects in batches instead of 1 at a time to keep the broad phase efficient.
    m_JoltPhysicsSystem->OptimizeBroadPhase();
    const uint32_t availableThreads = std::thread::hardware_concurrency() - 1;
    m_JoltJobSystem = new JPH::JobSystemThreadPool(JPH::cMaxPhysicsJobs, JPH::cMaxPhysicsBarriers, availableThreads);
}

void DynamicWorld::SetGravity(const glm::vec3 &gravity)
{
    // m_JoltPhysicsSystem->SetGravity(JPH::Vec3(gravity.x, gravity.y, gravity.z));
}

void DynamicWorld::AddRigidBody(RigidBodyRef rb)
{
    JPH::BodyInterface &bodyInterface = m_JoltPhysicsSystem->GetBodyInterface();

    const float mass = rb->Mass;
    JPH::EMotionType motionType = JPH::EMotionType::Static;

    // According to jolt documentation, Mesh shapes should only be static.
    const bool isMeshShape = rb->GetShape()->GetType() == RigidBodyShapes::MESH;
    if (mass > 0.0f && !isMeshShape)
    {
        if (rb->IsKinematic)
            motionType = JPH::EMotionType::Kinematic;
        else
            motionType = JPH::EMotionType::Dynamic;
    }
    if (rb->MotionType == MotionType::Static) motionType = JPH::EMotionType::Static;

    const auto &startPos = rb->GetPosition();
    const auto &bodyRotation = rb->GetRotation();
    const auto &joltRotation = JPH::Quat(bodyRotation.x, bodyRotation.y, bodyRotation.z, bodyRotation.w);
    const auto &joltPos = JPH::Vec3(startPos.x, startPos.y, startPos.z);
    auto joltShape = GetJoltShape(rb->GetShape());

    JPH::BodyCreationSettings bodySettings(joltShape, joltPos, joltRotation, motionType, Layers::MOVING);
    bodySettings.mLinearDamping = rb->LinearDamping;
    bodySettings.mAngularDamping = rb->AngularDamping;
    if (mass > 0.0f)
    {
        bodySettings.mOverrideMassProperties = JPH::EOverrideMassProperties::CalculateInertia;
        bodySettings.mMassPropertiesOverride.mMass = mass;
    }

    bodySettings.mUserData = (uint64_t)rb->GetEntity().GetEntityID();
    // Create the actual rigid body
    JPH::BodyID body = m_JoltBodyInterface->CreateAndAddBody(bodySettings, JPH::EActivation::Activate);
    m_RegisteredBodies.push_back((uint32_t)body.GetIndexAndSequenceNumber());
}

JPH::Ref<JPH::Shape> DynamicWorld::GetJoltShape(const PhysicShapeRef shape)
{
    JPH::ShapeSettings::ShapeResult result;

    switch (shape->GetType())
    {
        case RigidBodyShapes::BOX:
        {
            Box *box = (Box *)shape.get();
            const glm::vec3 &boxSize = box->GetSize();
            JPH::BoxShapeSettings shapeSettings(JPH::Vec3(boxSize.x, boxSize.y, boxSize.z));
            result = shapeSettings.Create();
        }
        break;
        case RigidBodyShapes::SPHERE:
        {
            Sphere *sphere = (Sphere *)shape.get();
            const float sphereRadius = sphere->GetRadius();
            JPH::SphereShapeSettings shapeSettings(sphereRadius);
            result = shapeSettings.Create();
        }
        break;
        case RigidBodyShapes::CAPSULE:
        {
            Capsule *capsule = (Capsule *)shape.get();
            const float radius = capsule->GetRadius();
            const float height = capsule->GetHeight();
            JPH::CapsuleShapeSettings shapeSettings(height / 2.0f, radius);
            result = shapeSettings.Create();
        }
        break;
        case RigidBodyShapes::CYLINDER:
        {
            Cylinder *capsule = (Cylinder *)shape.get();
            const float radius = capsule->GetRadius();
            const float height = capsule->GetHeight();
            JPH::CylinderShapeSettings shapeSettings(height / 2.0f, radius);
            result = shapeSettings.Create();
        }
        break;
        case RigidBodyShapes::MESH:
        {
            MeshShape *meshShape = (MeshShape *)shape.get();
            const auto &mesh = meshShape->GetMesh();
            const auto &vertices = mesh->Vertices;
            const auto &indices = mesh->Indices;

            JPH::TriangleList triangles;
            triangles.reserve(indices.size());
            auto transform = glm::mat4(1.0f);
            transform[3] = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
            for (int i = 0; i < indices.size() - 3; i += 3)
            {
                const glm::vec3 &p1 = vertices[indices[i]].Position;
                const glm::vec3 &p2 = vertices[indices[i + 1]].Position;
                const glm::vec3 &p3 = vertices[indices[i + 2]].Position;

                const glm::vec4 &tp1 = transform * glm::vec4(p1, 1.0f);
                const glm::vec4 &tp2 = transform * glm::vec4(p2, 1.0f);
                const glm::vec4 &tp3 = transform * glm::vec4(p3, 1.0f);
                triangles.push_back(JPH::Triangle(JPH::Float3(tp1.x, tp1.y, tp1.z), JPH::Float3(tp2.x, tp2.y, tp2.z),
                                                  JPH::Float3(tp3.x, tp3.y, tp3.z)));
            }

            JPH::MeshShapeSettings shapeSettings(std::move(triangles));

            result = shapeSettings.Create();
        }
        break;
        case RigidBodyShapes::CONVEX_HULL:
        {
            auto *convexHullShape = (Physics::ConvexHullShape *)shape.get();
            const auto &hullPoints = convexHullShape->GetPoints();
            JPH::Array<JPH::Vec3> points;
            points.reserve(std::size(hullPoints));
            for (const auto &p : hullPoints)
            {
                points.push_back(JPH::Vec3(p.x, p.y, p.z));
            }

            JPH::ConvexHullShapeSettings shapeSettings(points);
            result = shapeSettings.Create();
        }
        break;
    }

    return result.Get();
}

void DynamicWorld::SyncEntitiesTransforms()
{
    auto &bodyInterface = m_JoltPhysicsSystem->GetBodyInterface();
    for (const auto &body : m_RegisteredBodies)
    {
        auto bodyId = static_cast<JPH::BodyID>(body);
        JPH::Vec3 position = bodyInterface.GetCenterOfMassPosition(bodyId);
        JPH::Vec3 velocity = bodyInterface.GetLinearVelocity(bodyId);
        JPH::Mat44 joltTransform = bodyInterface.GetWorldTransform(bodyId);
        const auto bodyRotation = bodyInterface.GetRotation(bodyId);

        glm::mat4 transform =
            glm::mat4(joltTransform(0, 0), joltTransform(1, 0), joltTransform(2, 0), joltTransform(3, 0),
                      joltTransform(0, 1), joltTransform(1, 1), joltTransform(2, 1), joltTransform(3, 1),
                      joltTransform(0, 2), joltTransform(1, 2), joltTransform(2, 2), joltTransform(3, 2),
                      joltTransform(0, 3), joltTransform(1, 3), joltTransform(2, 3), joltTransform(3, 3));

        glm::vec3 scale = glm::vec3();
        glm::quat rotation = glm::quat();
        glm::vec3 pos = glm::vec3();
        glm::vec3 skew = glm::vec3();
        glm::vec4 pesp = glm::vec4();
        glm::decompose(transform, scale, rotation, pos, skew, pesp);

        auto entId = static_cast<int>(bodyInterface.GetUserData(bodyId));
        Entity entity = Entity{(entt::entity)entId, m_Scene};

        auto &transformComponent = entity.GetComponent<TransformComponent>();

        // update transform
        transformComponent.Translation = pos;
        transformComponent.Rotation = glm::vec3(rotation.x, rotation.y, rotation.z);
        // transformComponent.Scale = scale;

        // transformComponent.SetTransform(transform);
    }
}

void DynamicWorld::SyncCharactersTransforms() {}

void DynamicWorld::StepSimulation(float dt)
{
    // next step
    ++m_StepCount;

    // If you take larger steps than 1 / 90th of a second you need to do multiple collision steps in order to keep the
    // simulation stable. Do 1 collision step per 1 / 60th of a second (round up).
    int collisionSteps = 1;
    constexpr float minStepDuration = 1.0f / 90.0f; // ? make this configurable
    constexpr int maxStepCount = 32;

    if (dt > minStepDuration)
    {
        LOG_CORE_WARN("Large step detected: {}", dt);
        collisionSteps = static_cast<float>(dt) / minStepDuration;
    }

    if (collisionSteps >= maxStepCount)
    {
        LOG_CORE_WARN("Very large step detected: {}", dt);
    }

    // Prevents having too many steps and running out of jobs
    collisionSteps = std::min(collisionSteps, maxStepCount);

    // step the world
    try
    {
        auto joltTempAllocator = std::make_shared<JPH::TempAllocatorMalloc>();

        // TODO: character stepping

        m_JoltPhysicsSystem->Update(dt, collisionSteps, joltTempAllocator.get(), m_JoltJobSystem);
    }
    catch (...)
    {
        LOG_CORE_CRITICAL("Failed to run simulation step!");
    }

    SyncEntitiesTransforms();
}

void DynamicWorld::Clear()
{
    m_StepCount = 0;

    if (!m_RegisteredBodies.empty())
    {
        m_JoltBodyInterface->RemoveBodies(reinterpret_cast<JPH::BodyID *>(m_RegisteredBodies.data()),
                                          m_RegisteredBodies.size());
        m_RegisteredBodies.clear();
    }

    if (!m_RegisteredCharacters.empty())
    {
        for (auto &character : m_RegisteredCharacters)
        {
            // character.second->RemoveFromPhysicsSystem();
        }

        m_RegisteredCharacters.clear();
    }
}

void DynamicWorld::AddForceToRigidBody(Entity entity, const glm::vec3 &force)
{
    auto &bodyInterface = m_JoltPhysicsSystem->GetBodyInterface();
    for (const auto &body : m_RegisteredBodies)
    {
        auto bodyId = static_cast<JPH::BodyID>(body);
        auto entityId = static_cast<uint32_t>(bodyInterface.GetUserData(bodyId));
        if (entityId == (uint32_t)entity.GetEntityID())
        {
            bodyInterface.AddForce(bodyId, JPH::Vec3(force.x, force.y, force.z));
            return;
        }
    }
}

void DynamicWorld::DrawDebug(Entity entity)
{
    const auto rigidBodyComponent = entity.GetComponent<RigidBodyComponent>();
    if (entity.HasComponent<BoxColliderComponent>())
    {
        auto model = MeshImporter::LoadModel(Utils::Path::GetAbsolute("/Resources/Models/Cube/scene.gltf"));
        auto mesh = model->GetMeshes()[0];
        mesh.VAO.Bind();

        mesh.VAO.Unbind();
    }
}
} // namespace Physics
} // namespace Engine