#include "CameraController.h"

#include <iostream>

namespace Engine
{
CameraController::CameraController(Camera &camera, float sensitivity, float movementSpeed)
    : m_Camera(camera), m_Sensitivity(sensitivity), m_MovementSpeed(movementSpeed)
{
    InputManager::Instance().RegisterMouseMovedCallback(std::bind(&CameraController::OnMouseMove, this,
                                                                  std::placeholders::_1, std::placeholders::_2,
                                                                  std::placeholders::_3, std::placeholders::_4));
}

void CameraController::OnUpdate(float deltaTime)
{
    float velocity = m_MovementSpeed * deltaTime;

    if (InputManager::Instance().IsKeyPressed(InputKey::W))
        m_Camera.SetPosition(m_Camera.GetPosition() + m_Camera.GetFront() * velocity);
    if (InputManager::Instance().IsKeyPressed(InputKey::S))
        m_Camera.SetPosition(m_Camera.GetPosition() - m_Camera.GetFront() * velocity);
    if (InputManager::Instance().IsKeyPressed(InputKey::A))
        m_Camera.SetPosition(m_Camera.GetPosition() - m_Camera.GetRight() * velocity);
    if (InputManager::Instance().IsKeyPressed(InputKey::D))
        m_Camera.SetPosition(m_Camera.GetPosition() + m_Camera.GetRight() * velocity);
}

void CameraController::OnMouseMove(double xPos, double yPos, double xOffset, double yOffset)
{
    m_Camera.SetYaw(m_Camera.GetYaw() + (xOffset * m_Sensitivity));
    m_Camera.SetPitch(m_Camera.GetPitch() + (yOffset * m_Sensitivity));
}
} // namespace Engine