#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera
{
  public:
    Camera(glm::vec3 position, glm::vec3 direction, glm::vec3 worldUp, float yaw, float pitch);
    Camera();
    ~Camera() = default;

    const glm::mat4 &GetViewMatrix() const { return m_ViewMatrix; }

    inline void SetPosition(const glm::vec3 &position)
    {
        m_Position = position;
        RecalculateViewMatrix();
    }
    const glm::vec3 &GetPosition() const { return m_Position; }

    inline void SetYaw(float yaw)
    {
        m_Yaw = yaw;
        RecalculateViewMatrix();
    }
    float GetYaw() const { return m_Yaw; }

    inline void SetPitch(float pitch)
    {
        m_Pitch = pitch;
        RecalculateViewMatrix();
    }
    float GetPitch() const { return m_Pitch; }

    const glm::vec3 &GetFront() const { return m_Front; }
    const glm::vec3 &GetUp() const { return m_Up; }
    const glm::vec3 &GetRight() const { return m_Right; }

  private:
    void RecalculateViewMatrix();

  private:
    glm::vec3 m_Position;
    glm::vec3 m_Front;
    glm::vec3 m_Up;
    glm::vec3 m_Right;
    glm::vec3 m_WorldUp;

    float m_Yaw;
    float m_Pitch;

    glm::mat4 m_ViewMatrix;
};
