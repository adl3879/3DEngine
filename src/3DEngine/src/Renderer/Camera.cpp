#include "Camera.h"

namespace Engine
{
Camera::Camera()
    : m_Position(glm::vec3(0.0f, 0.0f, 0.3f)), m_Front(0.0f, 0.0f, -1.0f), m_WorldUp(0.0f, 1.0f, 0.0f), m_Yaw(-90.0f),
      m_Pitch(0.0f)
{
    RecalculateViewMatrix();
}

Camera::Camera(glm::vec3 position, glm::vec3 direction, glm::vec3 worldUp, float yaw, float pitch)
    : m_Position(position), m_Front(direction), m_WorldUp(worldUp), m_Yaw(yaw), m_Pitch(pitch)
{
    RecalculateViewMatrix();
}

void Camera::RecalculateViewMatrix()
{
    glm::vec3 front;
    front.x = cos(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
    front.y = sin(glm::radians(m_Pitch));
    front.z = sin(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));

    m_Front = glm::normalize(front);
    m_Right = glm::normalize(glm::cross(m_Front, m_WorldUp));
    m_Up = glm::normalize(glm::cross(m_Right, m_Front));

    m_ViewMatrix = glm::lookAt(m_Position, m_Position + m_Front, m_Up);
}
}