#pragma once

#include <glm/glm.hpp>
#include <memory>

#include "Entity.h"
#include "PhysicsShapes.h"

namespace Engine
{
namespace Physics
{
enum class MotionType
{
    Static,
    Dynamic,
};

class RigidBody
{
public:
    RigidBody() = default;
    RigidBody(glm::vec3 position, Entity handle);
    RigidBody(float mass, glm::vec3 position, glm::quat rotation, glm::mat4 transform, PhysicShapeRef shape,
              Entity entity, glm::vec3 initialValue = glm::vec3(0.0f));

    void UpdateTransform();

    void SetEntityID(Entity ent) { m_Entity = ent; }

    [[nodiscard]] bool HasShape() const { return m_CollisionShape != nullptr; }
    void SetShape(PhysicShapeRef shape);
    void AddForce(const glm::vec3 &force);

    [[nodiscard]] PhysicShapeRef GetShape() const { return m_CollisionShape; }
    [[nodiscard]] Entity GetEntity() const { return m_Entity; }
    [[nodiscard]] glm::vec3 GetPosition() const { return m_Position; }
    [[nodiscard]] glm::quat GetRotation() const { return m_Rotation; }
    [[nodiscard]] glm::vec3 GetScale() const { return m_Scale; }

public:
    float Mass;
    glm::mat4 Transform;

    MotionType MotionType;
    float LinearDamping;
    float AngularDamping;
    bool IsKinematic;
    bool UseGravity;

private:
    PhysicShapeRef m_CollisionShape;
    glm::vec3 m_Position = glm::vec3(0.0f);
    glm::quat m_Rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
    glm::vec3 m_Scale = glm::vec3(1.0f);
    Entity m_Entity;
};

static std::string MotionTypeToString(MotionType type)
{
    switch (type)
    {
        case MotionType::Static: return "Static";
        case MotionType::Dynamic: return "Dynamic";
        default: return "Unknown";
    }
}

static MotionType MotionTypeFromString(std::string type)
{
    if (type == "Static") return MotionType::Static;
    if (type == "Dynamic") return MotionType::Dynamic;
    return MotionType::Static;
}

using RigidBodyRef = std::shared_ptr<RigidBody>;
} // namespace Physics
} // namespace Engine
