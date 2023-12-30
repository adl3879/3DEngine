#include "RigidBody.h"

namespace Engine
{
namespace Physics
{
RigidBody::RigidBody(glm::vec3 position, Entity handle)
{
}

RigidBody::RigidBody(float mass, glm::vec3 position, glm::quat rotation, glm::mat4 transform, PhysicShapeRef shape,
                     Entity entity, glm::vec3 initialValue)
    : m_Position(position), m_CollisionShape(shape), Mass(mass), m_Entity(entity), Transform(transform),
      m_Rotation(rotation)
{
}

void RigidBody::UpdateTransform()
{
}

void RigidBody::SetShape(PhysicShapeRef shape) { m_CollisionShape = shape; }

void RigidBody::AddForce(const glm::vec3 &force)
{
}
} // namespace Physics
} // namespace Engine
