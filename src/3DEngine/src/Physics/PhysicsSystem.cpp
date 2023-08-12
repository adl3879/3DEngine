#include "PhysicsSystem.h"

#include "Layers.h"
#include "EventListener.h"

namespace Engine
{
using namespace JPH;

BodyInterface *bodyInterface = nullptr;

// Callback for traces, connect this to your own trace function if you have one
static void TraceImpl(const char *inFMT, ...)
{
    // Format the message
    va_list list;
    va_start(list, inFMT);
    char buffer[1024];
    vsnprintf(buffer, sizeof(buffer), inFMT, list);
    va_end(list);

    // Print to the TTY
    LOG_CORE_INFO(buffer);
}

#ifdef JPH_ENABLE_ASSERTS
// Callback for asserts, connect this to your own assert handler if you have one
static bool AssertFailedImpl(const char *inExpression, const char *inMessage, const char *inFile, uint inLine)
{
    LOG_CORE_INFO("{0}:{1}: ({2}) {3}", inFile, inLine, inExpression, inMessage != nullptr ? inMessage : "}");
    // Breakpoint
    return true;
};
#endif // JPH_ENABLE_ASSERTS

PhysicsSystem::~PhysicsSystem()
{
    delete m_JobSystem;
    delete m_TempAllocator;
}

void PhysicsSystem::Init()
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

    // We need a temp allocator for temporary allocations during the physics update. We're
    // pre-allocating 10 MB to avoid having to do allocations during the physics update.
    // B.t.w. 10 MB is way too much for this example but it is a typical value you can use.
    // If you don't want to pre-allocate you can also use TempAllocatorMalloc to fall back to
    // malloc / free.
    TempAllocatorImpl temp_allocator(10 * 1024 * 1024);

    // We need a job system that will execute physics jobs on multiple threads. Typically
    // you would implement the JobSystem interface yourself and let Jolt Physics run on top
    // of your own job scheduler. JobSystemThreadPool is an example implementation.
    JobSystemThreadPool job_system(cMaxPhysicsJobs, cMaxPhysicsBarriers, thread::hardware_concurrency() - 1);

    // This is the max amount of rigid bodies that you can add to the physics system. If you try to add more you'll get
    // an error. Note: This value is low because this is a simple test. For a real project use something in the order of
    // 65536.
    const uint cMaxBodies = 1024;

    // This determines how many mutexes to allocate to protect rigid bodies from concurrent access. Set it to 0 for the
    // default settings.
    const uint cNumBodyMutexes = 0;

    // This is the max amount of body pairs that can be queued at any time (the broad phase will detect overlapping
    // body pairs based on their bounding boxes and will insert them into a queue for the narrowphase). If you make this
    // buffer too small the queue will fill up and the broad phase jobs will start to do narrow phase work. This is
    // slightly less efficient. Note: This value is low because this is a simple test. For a real project use something
    // in the order of 65536.
    const uint cMaxBodyPairs = 1024;

    // This is the maximum size of the contact constraint buffer. If more contacts (collisions between bodies) are
    // detected than this number then these contacts will be ignored and bodies will start interpenetrating / fall
    // through the world. Note: This value is low because this is a simple test. For a real project use something in the
    // order of 10240.
    const uint cMaxContactConstraints = 1024;

    // Create mapping table from object layer to broadphase layer
    // Note: As this is an interface, PhysicsSystem will take a reference to this so this instance needs to stay alive!
    BPLayerInterfaceImpl broad_phase_layer_interface;

    // Create class that filters object vs broadphase layers
    // Note: As this is an interface, PhysicsSystem will take a reference to this so this instance needs to stay alive!
    ObjectVsBroadPhaseLayerFilterImpl object_vs_broadphase_layer_filter;

    // Create class that filters object vs object layers
    // Note: As this is an interface, PhysicsSystem will take a reference to this so this instance needs to stay alive!
    ObjectLayerPairFilterImpl object_vs_object_layer_filter;

    // Now we can create the actual physics system.
    JPH::PhysicsSystem physics_system;
    physics_system.Init(cMaxBodies, cNumBodyMutexes, cMaxBodyPairs, cMaxContactConstraints, broad_phase_layer_interface,
                        object_vs_broadphase_layer_filter, object_vs_object_layer_filter);

    // A contact listener gets notified when bodies (are about to) collide, and when they separate again.
    // Note that this is called from a job so whatever you do here needs to be thread safe.
    // Registering one is entirely optional.
    MyContactListener contact_listener;
    physics_system.SetContactListener(&contact_listener);

    // The main way to interact with the bodies in the physics system is through the body interface. There is a locking
    // and a non-locking variant of this. We're going to use the locking version (even though we're not planning to
    // access bodies from multiple threads)
    BodyInterface &body_interface = physics_system.GetBodyInterface();

    // Next we can create a rigid body to serve as the floor, we make a large box
    // Create the settings for the collision volume (the shape).
    // Note that for simple shapes (like boxes) you can also directly construct a BoxShape.
    BoxShapeSettings floor_shape_settings(Vec3(100.0f, 1.0f, 100.0f));

    // Create the shape
    ShapeSettings::ShapeResult floor_shape_result = floor_shape_settings.Create();
    ShapeRefC floor_shape =
        floor_shape_result
            .Get(); // We don't expect an error here, but you can check floor_shape_result for HasError() / GetError()

    // Create the settings for the body itself. Note that here you can also set other properties like the restitution /
    // friction.
    BodyCreationSettings floor_settings(floor_shape, RVec3(0.0, -1.0, 0.0), Quat::sIdentity(), EMotionType::Static,
                                        Layers::NON_MOVING);

    // Create the actual rigid body
    Body *floor =
        body_interface.CreateBody(floor_settings); // Note that if we run out of bodies this can return nullptr

    // Add it to the world
    body_interface.AddBody(floor->GetID(), EActivation::DontActivate);

    // Now create a dynamic body to bounce on the floor
    // Note that this uses the shorthand version of creating and adding a body to the world
    BodyCreationSettings sphere_settings(new SphereShape(0.5f), RVec3(0.0, 2.0, 0.0), Quat::sIdentity(),
                                         EMotionType::Dynamic, Layers::MOVING);
    BodyID sphere_id = body_interface.CreateAndAddBody(sphere_settings, EActivation::Activate);

    // Now you can interact with the dynamic body, in this case we're going to give it a velocity.
    // (note that if we had used CreateBody then we could have set the velocity straight on the body before adding it to
    // the physics system)
    body_interface.SetLinearVelocity(sphere_id, Vec3(0.0f, -5.0f, 0.0f));

    // We simulate the physics world in discrete time steps. 60 Hz is a good rate to update the physics system.
    const float cDeltaTime = 1.0f / 60.0f;

    // Optional step: Before starting the physics simulation you can optimize the broad phase. This improves collision
    // detection performance (it's pointless here because we only have 2 bodies). You should definitely not call this
    // every frame or when e.g. streaming in a new level section as it is an expensive operation. Instead insert all new
    // objects in batches instead of 1 at a time to keep the broad phase efficient.
    physics_system.OptimizeBroadPhase();
}

void PhysicsSystem::Shutdown() {}

void PhysicsSystem::Update(float deltaTime)
{
    auto &bodyInterface = m_PhysicsSystem.GetBodyInterface();
    const int cCollisionSteps = 1;
    // We simulate the physics world in discrete time steps. 60 Hz is a good rate to update the physics system.
    const float cDeltaTime = 1.0f / 60.0f;

    for (const auto &body : m_Bodies)
    {
        if (bodyInterface.IsActive(body))
            m_PhysicsSystem.Update(cDeltaTime, cCollisionSteps, m_TempAllocator, m_JobSystem);
    }
}

BodyID PhysicsSystem::CreateSphereBody(const glm::vec3 &position, float radius, float mass)
{

    // // Now create a dynamic body to bounce on the floor
    // // Note that this uses the shorthand version of creating and adding a body to the world
    // BodyCreationSettings sphereSettings(new SphereShape(radius), RVec3(position.x, position.y, position.z),
    //                                     Quat::sIdentity(), EMotionType::Static, Layers::MOVING);

    // BodyID sphereId = bodyInterface.CreateAndAddBody(sphereSettings, EActivation::Activate);

    // m_Bodies.push_back(sphereId);
    // return sphereId;
}

void PhysicsSystem::CreateBoxBody(const glm::vec3 &position, const glm::vec3 &size, float mass)
{
    BoxShapeSettings floor_shape_settings(Vec3(100.0f, 1.0f, 100.0f));

    // Create the shape
    ShapeSettings::ShapeResult floor_shape_result = floor_shape_settings.Create();
    ShapeRefC floor_shape = floor_shape_result.Get();

    // Create the settings for the body itself. Note that here you can also set other properties like the restitution /
    // friction.
    BodyCreationSettings floor_settings(floor_shape, RVec3(position.x, position.y, position.z), Quat::sIdentity(),
                                        EMotionType::Dynamic, Layers::MOVING);

    // Create the actual rigid body
    Body *floor = bodyInterface->CreateBody(floor_settings);

    // Add it to the world
    bodyInterface->AddBody(floor->GetID(), EActivation::DontActivate);
}
} // namespace Engine