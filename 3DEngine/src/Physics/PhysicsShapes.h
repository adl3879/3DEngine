#pragma once

#include <glm/glm.hpp>
#include <memory>
#include <vector>

#include "StaticMesh.h"

namespace Engine
{
namespace Physics
{
enum class RigidBodyShapes
{
    BOX = 0,
    SPHERE,
    CAPSULE,
    MESH,
    CYLINDER,
    CONVEX_HULL
};

class PhysicShape
{
protected:
    RigidBodyShapes m_Type;

public:
    [[nodiscard]] RigidBodyShapes GetType() const { return m_Type; }
};

class Box : public PhysicShape
{
private:
    glm::vec3 Size;

public:
    Box()
        : Size(glm::vec3(1.0f)) { m_Type = RigidBodyShapes::BOX; };

    Box(glm::vec3 size)
        : Size(size) { m_Type = RigidBodyShapes::BOX; };

    Box(float x, float y, float z)
        : Size(glm::vec3(x, y, z)) { m_Type = RigidBodyShapes::BOX; };

    [[nodiscard]] glm::vec3 GetSize() const { return Size; }
};

class Sphere : public PhysicShape
{
private:
    float Radius;

public:
    Sphere(float radius)
        : Radius(radius) { m_Type = RigidBodyShapes::SPHERE; };

    [[nodiscard]] float GetRadius() const { return Radius; }
    void SetRadius(float radius) { Radius = radius; };
};

class Capsule : public PhysicShape
{
private:
    float Radius;
    float Height;

public:
    Capsule() = default;

    Capsule(float radius, float height)
        : Radius(radius), Height(height) { m_Type = RigidBodyShapes::CAPSULE; }

    [[nodiscard]] float GetRadius() const { return Radius; }
    void SetRadius(float radius) { Radius = radius; }
    [[nodiscard]] float GetHeight() const { return Height; }
    void SetHeight(float height) { Height = height; }
};

class Cylinder : public PhysicShape
{
private:
    float Radius;
    float Height;

public:
    Cylinder(float radius, float height)
        : Radius(radius), Height(height) { m_Type = RigidBodyShapes::CYLINDER; }

    [[nodiscard]] float GetRadius() const { return Radius; }
    void SetRadius(float radius) { Radius = radius; }
    [[nodiscard]] float GetHeight() const { return Height; }
    void SetHeight(float height) { Height = height; }
};

class MeshShape : public PhysicShape
{
private:
    std::shared_ptr<StaticMesh> m_Mesh;

public:
    MeshShape(std::shared_ptr<StaticMesh> mesh)
        : m_Mesh(mesh) { m_Type = RigidBodyShapes::MESH; }

    void SetMesh(std::shared_ptr<StaticMesh> mesh) { m_Mesh = mesh; }
    [[nodiscard]] std::shared_ptr<StaticMesh> GetMesh() const { return m_Mesh; }
};

class ConvexHullShape : public PhysicShape
{
private:
    std::vector<glm::vec3> m_Points;

public:
    ConvexHullShape(const std::vector<glm::vec3> &points)
        : m_Points(points) { m_Type = RigidBodyShapes::CONVEX_HULL; }

    [[nodiscard]] std::vector<glm::vec3> GetPoints() const { return m_Points; }
    void SetPoints(const std::vector<glm::vec3> &points) { m_Points = points; }
};

using PhysicShapeRef = std::shared_ptr<PhysicShape>;

using BoxRef = std::shared_ptr<Box>;
using SphereRef = std::shared_ptr<Sphere>;
using CapsuleRef = std::shared_ptr<Capsule>;
using CylinderRef = std::shared_ptr<Cylinder>;
using MeshRef = std::shared_ptr<MeshShape>;
using ConvexHullRef = std::shared_ptr<ConvexHullShape>;
} // namespace Physics
} // namespace Engine
