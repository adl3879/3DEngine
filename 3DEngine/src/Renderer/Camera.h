#pragma once

#include <glm/glm.hpp>
#include <memory>

#include "Texture2D.h"

namespace Engine
{
class Scene;

class Camera
{
  public:
    virtual glm::mat4 GetProjectionViewMatrix() = 0;
    virtual glm::vec3 GetPosition() = 0;
    virtual void SetPosition(const glm::vec3 &position) = 0;
    virtual glm::mat4 GetProjectionMatrix() = 0;
    virtual glm::mat4 GetViewMatrix() = 0;

    virtual Texture2DRef GetPreviewTexture(Scene *scene) { return nullptr; }
};
using CameraRef = std::shared_ptr<Camera>;
} // namespace Engine