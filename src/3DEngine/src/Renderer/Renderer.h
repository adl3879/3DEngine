#pragma once

#include <glm/glm.hpp>

#include "Camera.h"
#include "Light.h"
#include "Model.h"

namespace Engine
{
class RendererCommand
{
  public:
    static void SetClearColor(const glm::vec4 &color);
    static void Clear();
};

class Renderer3D
{
  public:
    static void Init();
    static void Shutdown();

    static void BeginScene(); // default camera
    static void BeginScene(const Camera &camera);
    static void BeginScene(const Camera &camera, const Light &light);
    static void EndScene();

    static void DrawModel(Model &model, Shader &shader);
    static void DrawModel(Model &model, Shader &shader, const glm::vec3 &position,
                          const glm::vec3 &rotation = glm::vec3(0.0f), const glm::vec3 &scale = glm::vec3(1.0f));
    static void DrawFloor(const glm::vec3 &position, const glm::vec3 &rotation = glm::vec3(0.0f),
                          const glm::vec3 &scale = glm::vec3(1.0f));
    static void DrawSkybox();
};
} // namespace Engine