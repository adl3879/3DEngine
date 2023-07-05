#pragma once

#include <glm/glm.hpp>

namespace Engine
{
class Camera
{
  public:
    virtual glm::mat4 GetProjectionViewMatrix() = 0;
    virtual glm::vec3 GetPosition() = 0;
    virtual void SetPosition(const glm::vec3 &position) = 0;
    virtual glm::mat4 GetProjectionMatrix() = 0;
    virtual glm::mat4 GetViewMatrix() = 0;
};
} // namespace Engine