#include "Camera.h"

#include "InputManager.h"

namespace Engine
{
Camera::Camera()
    : m_Position(glm::vec3(0.0f, 0.0f, 0.3f)), m_Front(0.0f, 0.0f, -1.0f), m_WorldUp(0.0f, 1.0f, 0.0f), m_Yaw(-90.0f),
      m_Pitch(0.0f), m_FieldOfView(45.0f), m_AspectRatio(16.0f / 9.0f), m_NearPlane(0.1f), m_FarPlane(100.0f)
{
    RecalculateProjectionViewMatrix();
}

Camera::Camera(glm::vec3 position, glm::vec3 direction, glm::vec3 worldUp, float yaw, float pitch, float fieldOfView,
               float aspectRatio, float nearPlane, float farPlane)
    : m_Position(position), m_Front(direction), m_WorldUp(worldUp), m_Yaw(yaw), m_Pitch(pitch),
      m_FieldOfView(fieldOfView), m_AspectRatio(aspectRatio), m_NearPlane(nearPlane), m_FarPlane(farPlane)
{
    RecalculateProjectionViewMatrix();
}

void Camera::RecalculateProjectionViewMatrix()
{
    glm::vec3 front;
    front.x = cos(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
    front.y = sin(glm::radians(m_Pitch));
    front.z = sin(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));

    m_Front = glm::normalize(front);
    m_Right = glm::normalize(glm::cross(m_Front, m_WorldUp));
    m_Up = glm::normalize(glm::cross(m_Right, m_Front));
    auto viewMatrix = glm::lookAt(m_Position, m_Position + m_Front, m_Up);

    auto windowState = InputManager::Instance().GetWindowState();
    m_AspectRatio = static_cast<float>(windowState.Width) / static_cast<float>(windowState.Height);
    auto projection = glm::perspective(m_FieldOfView, m_AspectRatio, m_NearPlane, m_FarPlane);

    m_ProjectionViewMatrix = projection * viewMatrix;
}
} // namespace Engine