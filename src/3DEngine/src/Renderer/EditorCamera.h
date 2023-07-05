#pragma once

#include <glm/glm.hpp>

#include "Camera.h"

namespace Engine
{
class EditorCamera : public Camera
{
  public:
    EditorCamera() = default;
    EditorCamera(float fov, float aspectRatio, float nearClip, float farClip);

    glm::mat4 GetProjectionViewMatrix() override { return m_ProjectionMatrix * m_ViewMatrix; }
    glm::vec3 GetPosition() override { return m_Position; }
    void SetPosition(const glm::vec3 &position) override { m_Position = position; }
    glm::mat4 GetProjectionMatrix() override { return m_ProjectionMatrix; }
    glm::mat4 GetViewMatrix() override { return m_ViewMatrix; }

    void OnUpdate(float ts);

    inline float GetDistance() const { return m_Distance; }
    inline void SetDistance(float distance) { m_Distance = distance; }

    inline void SetViewportSize(float width, float height)
    {
        m_ViewportWidth = width;
        m_ViewportHeight = height;
        UpdateProjection();
    }

    const glm::mat4 &GetViewMatrix() const { return m_ViewMatrix; }
    const glm::mat4 &GetProjectionMatrix() const { return m_ProjectionMatrix; }

    glm::vec3 GetUpDirection() const;
    glm::vec3 GetRightDirection() const;
    glm::vec3 GetForwardDirection() const;
    glm::quat GetOrientation() const;

    float GetPitch() const { return m_Pitch; }
    float GetYaw() const { return m_Yaw; }

    void OnMouseScrolled(double xOffset, double yOffset);

  private:
    void UpdateProjection();
    void UpdateView();

    void MousePan(const glm::vec2 &delta);
    void MouseRotate(const glm::vec2 &delta);
    void MouseZoom(float delta);

    glm::vec3 CalculatePosition() const;

    std::pair<float, float> PanSpeed() const;
    float RotationSpeed() const;
    float ZoomSpeed() const;

  private:
    float m_FOV = 45.0f, m_AspectRatio = 1.778f, m_NearClip = 0.1f, m_FarClip = 1000.0f;

    glm::mat4 m_ViewMatrix, m_ProjectionMatrix;
    glm::vec3 m_Position = {0.0f, 0.0f, 0.0f};
    glm::vec3 m_FocalPoint = {0.0f, 0.0f, 0.0f};

    glm::vec2 m_InitialMousePosition = {0.0f, 0.0f};

    float m_Distance = 10.0f;
    float m_Pitch = 0.0f, m_Yaw = 0.0f;

    float m_ViewportWidth = 1280, m_ViewportHeight = 720;
};

} // namespace Engine