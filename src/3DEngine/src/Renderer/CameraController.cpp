#include "CameraController.h"

#include <iostream>

namespace Engine
{
MouseMovedPosition lastMousePos;

CameraController::CameraController(Camera &camera, float sensitivity, float movementSpeed)
    : m_Camera(camera), m_Sensitivity(sensitivity), m_MovementSpeed(movementSpeed)
{
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

    auto mousePos = InputManager::Instance().GetMouseMovedPosition();
    float xOff = mousePos.X - lastMousePos.X;
    float yOff = lastMousePos.Y - mousePos.Y;
    lastMousePos = mousePos;

    m_Camera.SetYaw(m_Camera.GetYaw() + (xOff * m_Sensitivity));
    m_Camera.SetPitch(m_Camera.GetPitch() + (-yOff * m_Sensitivity));
}

} // namespace Engine