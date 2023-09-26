#include "Environment.h"

namespace Engine
{
Environment::Environment()
{
    AmbientColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    ProceduralSkybox = std::make_shared<ProceduralSky>();

	Bloom = std::make_shared<BloomRenderer>();
	Bloom->Init(1280, 720);
}
} // namespace Engine
