#include "InfiniteGrid.h"

#include <glad/glad.h>

#include "ShaderManager.h"
#include "RenderCommand.h"
#include "Renderer.h"

namespace Engine
{
unsigned int VAO, VBO;

void InfiniteGrid::Init() 
{
}

void InfiniteGrid::Draw(glm::mat4 projection, glm::mat4 view, glm::vec3 cameraPos) 
{
    RenderCommand::Enable(RendererEnum::BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	Shader *skyShader = ShaderManager::GetShader("Resources/shaders/infiniteGrid");
	skyShader->Bind();

	skyShader->SetUniformMatrix4fv("Projection", projection);
	skyShader->SetUniformMatrix4fv("View", view);
	skyShader->SetUniform3f("CameraPos", cameraPos);

	Renderer::DrawQuad();

	RenderCommand::Disable(RendererEnum::BLEND);
}
} // namespace Engine
