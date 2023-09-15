#pragma once

#include <glm/fwd.hpp>
#include <glm/glm.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/matrix_transform.hpp>

#include <glm/gtc/type_ptr.hpp>
#include <memory>

#include "Camera.h"

namespace Engine
{
class PerspectiveCamera : public Camera
{
  public:
    PerspectiveCamera();
    ~PerspectiveCamera() = default;

    void RecalculateProjectionMatrix();
    void OnUpdate() { RecalculateProjectionMatrix(); }
    void OnWindowResize(float width, float height);

    glm::mat4 GetProjectionViewMatrix() override;
    glm::vec3 GetPosition() override { return m_Position; }
    void SetPosition(const glm::vec3 &position) override { m_Position = position; }
    glm::mat4 GetProjectionMatrix() override;
    glm::mat4 GetViewMatrix() override;

  public:
    inline void SetRotation(const glm::vec3 &rotation) { m_Rotation = rotation; }
    const glm::vec3 &GetRotation() const { return m_Rotation; }

  public:
    float GetPerspectiveVerticalFOV() const { return m_PerspectiveVerticalFOV; }
    void SetPerspectiveVerticalFOV(float verticalFov) { m_PerspectiveVerticalFOV = verticalFov; }
    float GetPerspectiveNearClip() const { return m_PerspectiveNearClip; }
    void SetPerspectiveNearClip(float nearClip) { m_PerspectiveNearClip = nearClip; }
    float GetPerspectiveFarClip() const { return m_PerspectiveFarClip; }
    void SetPerspectiveFarClip(float farClip) { m_PerspectiveFarClip = farClip; }

  private:
    glm::vec3 m_Position, m_Front, m_Up, m_Right, m_WorldUp;
    glm::vec3 m_Rotation;

    glm::mat4 m_ProjectionMatrix;
    glm::mat4 m_ViewMatrix;

    float m_FieldOfView, m_AspectRatio, m_NearPlane, m_FarPlane;

    float m_PerspectiveVerticalFOV;
    float m_PerspectiveNearClip, m_PerspectiveFarClip;
};

using PerspectiveCameraRef = std::shared_ptr<PerspectiveCamera>;
} // namespace Engine
