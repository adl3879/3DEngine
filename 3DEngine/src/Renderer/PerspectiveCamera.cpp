#include "PerspectiveCamera.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#include "InputManager.h"
#include "SceneRenderer.h"

namespace Engine
{
PerspectiveCamera::PerspectiveCamera()
    : m_Position(glm::vec3(0.0f, 0.0f, -5.0f)), m_Front(0.0f, 0.0f, 1.0f), m_WorldUp(0.0f, 1.0f, 0.0f),
      m_AspectRatio(16.0f / 9.0f), m_Yaw(-90.0f), m_Pitch(0.0f)
{
    m_PerspectiveVerticalFOV = glm::radians(-45.0f);
    m_PerspectiveNearClip = 0.1f;
    m_PerspectiveFarClip = 100.0f;

    m_Framebuffer = std::make_shared<Framebuffer>(true, glm::vec2{1280 / 2, 720 / 2});
    m_Framebuffer->SetTexture(std::make_shared<Texture2D>(ImageFormat::Depth), GL_DEPTH_ATTACHMENT);
    m_Framebuffer->SetTexture(std::make_shared<Texture2D>(ImageFormat::RGBA8), GL_COLOR_ATTACHMENT0);

    m_SceneRenderer = new SceneRenderer();
    m_SceneRenderer->Init();
    m_SceneRenderer->SetShowDebug(false);
}

void PerspectiveCamera::RecalculateProjectionMatrix()
{
    auto orientation = glm::quat(m_Rotation);

    glm::vec3 front = glm::vec3();
    // calculate the front vector from the Camera's (updated) Euler Angles
    front.x = cos(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
    front.y = sin(glm::radians(m_Pitch));
    front.z = sin(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));

    front = glm::normalize(orientation * front);

    m_Front = glm::normalize(front);
    m_Right = glm::normalize(glm::cross(m_Front, m_WorldUp));
    m_Up = glm::normalize(glm::cross(m_Right, m_Front));
    m_ViewMatrix = glm::lookAt(m_Position, m_Position + m_Front, m_Up);

    m_ProjectionMatrix =
        glm::perspective(m_PerspectiveVerticalFOV, m_AspectRatio, m_PerspectiveNearClip, m_PerspectiveFarClip);
}

void PerspectiveCamera::OnWindowResize(float width, float height) { m_AspectRatio = width / height; }

glm::mat4 PerspectiveCamera::GetProjectionViewMatrix()
{
    RecalculateProjectionMatrix();
    return m_ProjectionMatrix * m_ViewMatrix;
}

glm::mat4 PerspectiveCamera::GetProjectionMatrix()
{
    RecalculateProjectionMatrix();
    return m_ProjectionMatrix;
}

glm::mat4 PerspectiveCamera::GetViewMatrix()
{
    RecalculateProjectionMatrix();
    return m_ViewMatrix;
}

Texture2DRef PerspectiveCamera::GetPreviewTexture(Scene *scene)
{
    m_SceneRenderer->BeginRenderScene(GetProjectionMatrix(), GetViewMatrix(), GetPosition());
    m_SceneRenderer->RenderScene(*scene, *m_Framebuffer);

    return m_Framebuffer->GetTexture();
}
} // namespace Engine
