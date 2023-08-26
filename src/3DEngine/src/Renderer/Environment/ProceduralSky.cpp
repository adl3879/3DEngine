#include "ProceduralSky.h"

#include <glad/glad.h>

#include "ShaderManager.h"
#include "RenderCommand.h"

namespace Engine
{
ProceduralSky::ProceduralSky()
{
    // clang-format off
    float quadVertices[] = {
		// positions        // texture Coords
		-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
		1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
		-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,

		1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
		1.0f,  1.0f, 0.0f, 1.0f, 1.0f
	};
    // clang-format on

    // setup plane VAO
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));
}

void ProceduralSky::Draw(glm::mat4 projection, glm::mat4 view)
{
    Shader *skyShader = ShaderManager::GetShader("/Resources/shaders/atmosphericSky");
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
    RenderCommand::Disable(RendererEnum::FACE_CULL);

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    RenderCommand::Enable(RendererEnum::DEPTH_TEST);
    RenderCommand::Enable(RendererEnum::FACE_CULL);
}
} // namespace Engine