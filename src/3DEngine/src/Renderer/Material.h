#pragma once

#include <glm/glm.hpp>

namespace Engine
{
struct Material
{
    glm::vec3 AmbientColor = {1.0f, 1.0f, 1.0f};
    glm::vec3 DiffuseColor = {1.0f, 1.0f, 1.0f};
    glm::vec3 SpecularColor = {1.0f, 1.0f, 1.0f};
};
} // namespace Engine