#include "InfiniteGrid.h"

#include <glad/glad.h>

#include "ShaderManager.h"

namespace Engine
{
unsigned int VAO, VBO;

void InfiniteGrid::Init() 
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

void InfiniteGrid::Draw(glm::mat4 projection, glm::mat4 view) 
{
	Shader *skyShader = ShaderManager::GetShader("Resources/shaders/infiniteGrid");
	skyShader->Bind();

	skyShader->SetUniformMatrix4fv("Projection", projection);
	skyShader->SetUniformMatrix4fv("View", view);

	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}
} // namespace Engine
