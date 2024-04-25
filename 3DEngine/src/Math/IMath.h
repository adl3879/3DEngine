#pragma once

#include <glm/glm.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/quaternion.hpp>

namespace Engine
{
namespace Math
{
bool DecomposeTransform(const glm::mat4 &transform, glm::vec3 &outTranslation, glm::vec3 &outRotation, glm::vec3 &outScale);
glm::vec3 ScreenToWorld(const glm::vec2 &mouse, const glm::vec2 &screenSize, const glm::mat4 &projection, const glm::mat4 &view);

glm::quat QuatFromEuler(float x, float y, float z);
glm::vec3 QuatToDirection(const glm::quat &quat);
}
} // namespace Engine
