#pragma once

#include <glm/glm.hpp>
#include <vector>

namespace Engine
{
struct Vertex
{
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec3 Color = glm::vec3{1.0f};
    glm::vec2 TexCoords;
    // editor only
    float EntityID = -1.0f;
};

struct VertexSOA
{
    std::vector<glm::vec3> Positions;
    std::vector<glm::vec3> Normals;
    std::vector<glm::vec3> Colors;
    std::vector<glm::vec2> TexCoords;
    std::vector<float> EntityIDs;
};
} // namespace Engine