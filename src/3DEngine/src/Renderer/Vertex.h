#pragma once

#include <glm/glm.hpp>

namespace Engine
{
struct Vertex
{
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec3 Color = glm::vec3{1.0f};
    glm::vec2 TexCoords;
    // editor only
    float EditorID;
};
} // namespace Engine