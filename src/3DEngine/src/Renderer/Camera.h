#pragma once

#include <glm/fwd.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Engine
{
class Camera
{
  public:
    Camera(glm::vec3 position, glm::vec3 direction, glm::vec3 worldUp, float yaw, float pitch, float fieldOfView,
           float aspectRatio, float nearPlane, float farPlane);
    Camera();
    ~Camera() = default;

    const glm::mat4 &GetProjectionViewMatrix() const { return m_ProjectionViewMatrix; }

    inline void SetPosition(const glm::vec3 &position)
    {
        m_Position = position;
        RecalculateProjectionViewMatrix();
    }
    const glm::vec3 &GetPosition() const { return m_Position; }

    inline void SetYaw(float yaw)
    {
        m_Yaw = yaw;
        RecalculateProjectionViewMatrix();
    }
    float GetYaw() const { return m_Yaw; }

    inline void SetPitch(float pitch)
    {
        m_Pitch = pitch;
        RecalculateProjectionViewMatrix();
    }
    float GetPitch() const { return m_Pitch; }

    inline void SetFieldOfView(float fieldOfView)
    {
        m_FieldOfView = fieldOfView;
        RecalculateProjectionViewMatrix();
    }
    float GetFieldOfView() const { return m_FieldOfView; }

    inline void SetAspectRatio(float aspectRatio)
    {
        m_AspectRatio = aspectRatio;
        RecalculateProjectionViewMatrix();
    }
    float GetAspectRatio() const { return m_AspectRatio; }

    inline void SetNearPlane(float nearPlane)
    {
        m_NearPlane = nearPlane;
        RecalculateProjectionViewMatrix();
    }
    float GetNearPlane() const { return m_NearPlane; }

    inline void SetFarPlane(float farPlane)
    {
        m_FarPlane = farPlane;
        RecalculateProjectionViewMatrix();
    }
    float GetFarPlane() const { return m_FarPlane; }

    const glm::vec3 &GetFront() const { return m_Front; }
    const glm::vec3 &GetUp() const { return m_Up; }
    const glm::vec3 &GetRight() const { return m_Right; }

  private:
    void RecalculateProjectionViewMatrix();

  private:
    glm::vec3 m_Position, m_Front, m_Up, m_Right, m_WorldUp;
    float m_Yaw, m_Pitch;
    float m_FieldOfView, m_AspectRatio, m_NearPlane, m_FarPlane;

    glm::mat4 m_ProjectionViewMatrix;
};
} // namespace Engine