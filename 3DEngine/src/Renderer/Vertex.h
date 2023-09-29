#pragma once

#include <glm/glm.hpp>
#include <vector>

namespace Engine
{
struct Vertex
{
    glm::vec3 Position;
    glm::vec2 TexCoords;
    glm::vec3 Normal;
	glm::vec3 Tangent;
	glm::vec3 Bitangent;
};
} // namespace Engine
