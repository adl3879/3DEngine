#include "ProceduralSky.h"

#include <glad/glad.h>

#include "ShaderManager.h"
#include "RenderCommand.h"
#include "Renderer.h"

namespace Engine
{
ProceduralSky::ProceduralSky() {}

void ProceduralSky::Draw(glm::mat4 projection, glm::mat4 view)
{
    Shader *skyShader = ShaderManager::GetShader("Resources/shaders/atmosphericSky");
    skyShader->Bind();
    skyShader->SetUniform1f("SurfaceRadius", SurfaceRadius);
    skyShader->SetUniform1f("AtmosphereRadius", AtmosphereRadius);
    skyShader->SetUniform1f("SunIntensity", SunIntensity);

    skyShader->SetUniform3f("RayleighScattering", RayleighScattering);
    skyShader->SetUniform3f("MieScattering", MieScattering);
    skyShader->SetUniform3f("CenterPoint", CenterPoint);
    skyShader->SetUniform3f("SunDirection", SunDirection);

    skyShader->SetUniformMatrix4fv("Projection", projection);
    skyShader->SetUniformMatrix4fv("View", view);

    RenderCommand::Disable(RendererEnum::DEPTH_TEST);

    Renderer::DrawQuad();

    RenderCommand::Enable(RendererEnum::DEPTH_TEST);
}
} // namespace Engine
