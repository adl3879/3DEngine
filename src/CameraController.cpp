#include "CameraController.h"
#include <iostream>

CameraController::CameraController(Camera &camera, float sensitivity, float movementSpeed)
    : m_Camera(camera), m_Sensitivity(sensitivity), m_MovementSpeed(movementSpeed)
{
    InputManager::Instance().RegisterCursorCallback([this](double xpos, double ypos) { OnMouseMove(xpos, ypos); });
}

void CameraController::OnUpdate(float deltaTime)
{
    float velocity = m_MovementSpeed * deltaTime;

    if (InputManager::Instance().IsKeyPressed(InputKey::W))
    {
        m_Camera.SetPosition(m_Camera.GetPosition() + m_Camera.GetFront() * velocity);
    }
    if (InputManager::Instance().IsKeyPressed(InputKey::S))
    {
        m_Camera.SetPosition(m_Camera.GetPosition() - m_Camera.GetFront() * velocity);
    }
    if (InputManager::Instance().IsKeyPressed(InputKey::A))
    {
        m_Camera.SetPosition(m_Camera.GetPosition() - m_Camera.GetRight() * velocity);
    }
    if (InputManager::Instance().IsKeyPressed(InputKey::D))
    {
        m_Camera.SetPosition(m_Camera.GetPosition() + m_Camera.GetRight() * velocity);
    }
}

double lastX = 0, lastY = 0;

void CameraController::OnMouseMove(double xpos, double ypos)
{
    double xoffset = xpos - lastX;
    double yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    xoffset *= m_Sensitivity;
    yoffset *= m_Sensitivity;

    m_Camera.SetYaw(m_Camera.GetYaw() + xoffset);
    m_Camera.SetPitch(m_Camera.GetPitch() + yoffset);
}
