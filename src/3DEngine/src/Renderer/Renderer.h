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

    static void DrawModel(Model &model);
    static void DrawModel(Model &model, const glm::mat4 &transform);
    static void DrawModel(Model &model, Shader &shader);

    static void DrawSkybox();
};
} // namespace Engine