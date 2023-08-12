#pragma once

#include "JoltPhysicsIncludes.h"
#include "Log.h"

#include <memory>
#include <glm/glm.hpp>

// disable common errors
JPH_SUPPRESS_WARNINGS

namespace Engine
{
enum class ShapeType : uint8_t
{
    Sphere = 0,
    Capsule,
    Cylinder,
    Box,
    ConvexHull,
    HeightField,
    Compound,
    StaticCompound,
    Invalid
};

class PhysicsSystem
{
  public:
    PhysicsSystem() = default;
    virtual ~PhysicsSystem();

    void Init();
    void Shutdown();
    void Update(float deltaTime);

    JPH::BodyInterface *GetBodyInterface() { return &m_PhysicsSystem.GetBodyInterface(); }

  public:
    JPH::BodyID CreateSphereBody(const glm::vec3 &position, float radius, float mass);
    void CreateBoxBody(const glm::vec3 &position, const glm::vec3 &size, float mass);

  private:
    JPH::PhysicsSystem m_PhysicsSystem;

    JPH::TempAllocatorImpl *m_TempAllocator = nullptr;
    JPH::JobSystemThreadPool *m_JobSystem = nullptr;

    // store all the shapes we create
    std::vector<JPH::Shape *> m_Shapes;
    std::vector<JPH::BodyID> m_Bodies;
};

using PhysicsSystemRef = std::shared_ptr<PhysicsSystem>;
} // namespace Engine