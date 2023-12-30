#pragma once

#include <string>
#include <glm/glm.hpp>

namespace Engine
{
const char *_labelPrefix(const char *const label, const char *field = "");
void _drawVec3Control(const std::string &label, glm::vec3 &values, float resetValue = 0.0f);
void _collapsingHeaderStyle(bool closed = false);
} // namespace Engine
