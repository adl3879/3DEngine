#include "Environment.h"

namespace Engine
{
Environment::Environment()
{
    AmbientColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    ProceduralSkybox = std::make_shared<ProceduralSky>();
}
} // namespace Engine
