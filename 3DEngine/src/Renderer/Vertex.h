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

const uint32_t MAX_BONE_INFLUENCE = 4;
struct SkinnedVertex
{
    glm::vec3 Position;
    glm::vec2 TexCoords;
    glm::vec3 Normal;
    glm::vec3 Tangent;
    glm::vec3 Bitangent;

	int BoneIDs[MAX_BONE_INFLUENCE];
    float Weights[MAX_BONE_INFLUENCE];
};
} // namespace Engine
