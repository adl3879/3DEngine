#include "InputKey.h"
#include "InputManager.h"

#include "Camera.h"

namespace Engine
{
class CameraController
{
  public:
    CameraController(Camera &camera, float sensitivity, float movementSpeed);
    ~CameraController() = default;

    void OnUpdate(float deltaTime);

  private:
    void OnMouseMove(double xoffset, double yoffset);

  private:
    Camera &m_Camera;
    float m_Sensitivity;
    float m_MovementSpeed;
};
}