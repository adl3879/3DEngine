#pragma once

#include <glm/fwd.hpp>
#include <glm/glm.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/matrix_transform.hpp>

#include <glm/gtc/type_ptr.hpp>
#include <memory>

#include "Camera.h"
#include "Framebuffer.h"

namespace Engine
{
class Scene;
class SceneRenderer;

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
    Texture2DRef GetPreviewTexture(Scene *scene) override;

    void ResizeFramebuffer(const glm::vec2 &size) { m_Framebuffer->QueueResize(size); }

  public:
    inline void SetRotation(const glm::vec3 &rotation) { m_Rotation = rotation; }
    [[nodiscard]] const glm::vec3 &GetRotation() const { return m_Rotation; }

  public:
    [[nodiscard]] float GetPerspectiveVerticalFOV() const { return m_PerspectiveVerticalFOV; }
    void SetPerspectiveVerticalFOV(float verticalFov) { m_PerspectiveVerticalFOV = verticalFov; }
    [[nodiscard]] float GetPerspectiveNearClip() const { return m_PerspectiveNearClip; }
    void SetPerspectiveNearClip(float nearClip) { m_PerspectiveNearClip = nearClip; }
    [[nodiscard]] float GetPerspectiveFarClip() const { return m_PerspectiveFarClip; }
    void SetPerspectiveFarClip(float farClip) { m_PerspectiveFarClip = farClip; }

  private:
    glm::vec3 m_Position, m_Front, m_Up, m_Right, m_WorldUp;
    glm::vec3 m_Rotation;
    float m_Yaw, m_Pitch;

    glm::mat4 m_ProjectionMatrix;
    glm::mat4 m_ViewMatrix;

    float m_AspectRatio;

    float m_PerspectiveVerticalFOV;
    float m_PerspectiveNearClip, m_PerspectiveFarClip;

  private:
    FramebufferRef m_Framebuffer;
    SceneRenderer *m_SceneRenderer;
};

using PerspectiveCameraRef = std::shared_ptr<PerspectiveCamera>;
} // namespace Engine
