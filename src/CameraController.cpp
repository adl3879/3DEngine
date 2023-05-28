#include "CameraController.h"

#include <iostream>

CameraController::CameraController(Camera &camera, float sensitivity, float movementSpeed)
    : m_Camera(camera), m_Sensitivity(sensitivity), m_MovementSpeed(movementSpeed)
{
    InputManager::Instance().RegisterCursorCallback([this](double xPos, double yPos) { OnMouseMove(xPos, yPos); });
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

double lastX = 0, lastY = 0;

void CameraController::OnMouseMove(double xPos, double yPos)
{
    double xOffset = xPos - lastX;
    double yOffset = lastY - yPos;
    lastX = xPos;
    lastY = yPos;

    xOffset *= m_Sensitivity;
    yOffset *= m_Sensitivity;

    m_Camera.SetYaw(m_Camera.GetYaw() + xOffset);
    m_Camera.SetPitch(m_Camera.GetPitch() + yOffset);
}
