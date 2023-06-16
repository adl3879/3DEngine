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
    enum class ProjectionType
    {
        Perspective = 0,
        Orthographic = 1
    };

  public:
    Camera();
    ~Camera() = default;

    ProjectionType GetProjectionType() const { return m_ProjectionType; }
    void SetProjectionType(ProjectionType projectionType) { m_ProjectionType = projectionType; }

    glm::mat4 GetProjectionViewMatrix();

    inline void SetPosition(const glm::vec3 &position) { m_Position = position; }
    const glm::vec3 &GetPosition() const { return m_Position; }

    inline void SetRotation(const glm::vec3 &rotation) { m_Rotation = rotation; }
    const glm::vec3 &GetRotation() const { return m_Rotation; }

    void SetPerspective(float verticalFov, float aspectRatio, float nearClip, float farClip);
    void SetOrthographic(float size, float nearClip, float farClip);

    float GetPerspectiveVerticalFOV() const { return m_PerspectiveVerticalFOV; }
    void SetPerspectiveVerticalFOV(float verticalFov) { m_PerspectiveVerticalFOV = verticalFov; }
    float GetPerspectiveNearClip() const { return m_PerspectiveNearClip; }
    void SetPerspectiveNearClip(float nearClip) { m_PerspectiveNearClip = nearClip; }
    float GetPerspectiveFarClip() const { return m_PerspectiveFarClip; }
    void SetPerspectiveFarClip(float farClip) { m_PerspectiveFarClip = farClip; }

    float GetOrthographicSize() const { return m_OrthographicSize; }
    void SetOrthographicSize(float size) { m_OrthographicSize = size; }
    float GetOrthographicNearClip() const { return m_OrthographicNearClip; }
    void SetOrthographicNearClip(float nearClip) { m_OrthographicNearClip = nearClip; }
    float GetOrthographicFarClip() const { return m_OrthographicFarClip; }
    void SetOrthographicFarClip(float farClip) { m_OrthographicFarClip = farClip; }

  private:
    glm::vec3 m_Position, m_Front, m_Up, m_Right, m_WorldUp;
    glm::vec3 m_Rotation = glm::vec3(-90.0f, 0.0f, 0.0f);
    float m_FieldOfView, m_AspectRatio, m_NearPlane, m_FarPlane;

    float m_PerspectiveVerticalFOV = glm::radians(280.0f);
    float m_PerspectiveNearClip = 0.01f, m_PerspectiveFarClip = 100.0f;

    float m_OrthographicSize = 1.0f;
    float m_OrthographicNearClip = -1.0f, m_OrthographicFarClip = 1.0f;

    ProjectionType m_ProjectionType = ProjectionType::Perspective;
};
} // namespace Engine