#pragma once

#include <glm/glm.hpp>

namespace Engine
{
class InfiniteGrid
{
  public:
	InfiniteGrid() = default;
	static void Init();
    static void Draw(glm::mat4 projection, glm::mat4 view, glm::vec3 cameraPos);
};
} // namespace Engine
