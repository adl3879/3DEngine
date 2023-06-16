#include "Camera.h"

#include "InputManager.h"

namespace Engine
{
Camera::Camera()
    : m_Position(glm::vec3(0.0f, 0.0f, 4.0f)), m_Front(0.0f, 0.0f, -1.0f), m_WorldUp(0.0f, 1.0f, 0.0f),
      m_FieldOfView(80.0f), m_AspectRatio(16.0f / 9.0f), m_NearPlane(0.1f), m_FarPlane(100.0f)
{
}

glm::mat4 Camera::GetProjectionViewMatrix()
{
    auto windowState = InputManager::Instance().GetWindowState();
    m_AspectRatio = static_cast<float>(windowState.Width) / static_cast<float>(windowState.Height);

    if (m_ProjectionType == ProjectionType::Orthographic)
    {
        float orthoLeft = -m_OrthographicSize * m_AspectRatio * 0.5f;
        float orthoRight = m_OrthographicSize * m_AspectRatio * 0.5f;
        float orthoBottom = -m_OrthographicSize * 0.5f;
        float orthoTop = m_OrthographicSize * 0.5f;

        glm::mat4 transform = glm::translate(glm::mat4(1.0f), m_Position) *
                              glm::rotate(glm::mat4(1.0f), glm::radians(m_Rotation.x), glm::vec3(0, 0, 1));
        auto viewMatrix = glm::inverse(transform);

        auto projection =
            glm::ortho(orthoLeft, orthoRight, orthoBottom, orthoTop, m_OrthographicNearClip, m_OrthographicFarClip);

        glm::mat4 projectionViewMatrix = projection * viewMatrix;

        return projectionViewMatrix;
    }
    else if (m_ProjectionType == ProjectionType::Perspective)
    {
        glm::vec3 front;
        // calculate the front vector from the Camera's (updated) Euler Angles
        front.x = cos(glm::radians(m_Rotation.x)) * cos(glm::radians(m_Rotation.y));
        front.y = sin(glm::radians(m_Rotation.y));
        front.z = sin(glm::radians(m_Rotation.x)) * cos(glm::radians(m_Rotation.y));

        m_Front = glm::normalize(front);
        m_Right = glm::normalize(glm::cross(m_Front, m_WorldUp));
        m_Up = glm::normalize(glm::cross(m_Right, m_Front));
        auto viewMatrix = glm::lookAt(m_Position, m_Position + m_Front, m_Up);

        auto projection =
            glm::perspective(m_PerspectiveVerticalFOV, m_AspectRatio, m_PerspectiveNearClip, m_PerspectiveFarClip);

        glm::mat4 projectionViewMatrix = projection * viewMatrix;
        return projectionViewMatrix;
    }
    return glm::mat4{};
}

void Camera::SetPerspective(float verticalFov, float aspectRatio, float nearClip, float farClip)
{
    m_ProjectionType = ProjectionType::Perspective;
    m_FieldOfView = verticalFov;
    m_AspectRatio = aspectRatio;
    m_NearPlane = nearClip;
    m_FarPlane = farClip;
}

void Camera::SetOrthographic(float size, float nearClip, float farClip)
{
    m_ProjectionType = ProjectionType::Orthographic;
    m_OrthographicSize = size;
    m_OrthographicNearClip = nearClip;
    m_OrthographicFarClip = farClip;
}
} // namespace Engine