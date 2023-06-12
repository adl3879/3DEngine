#include "Renderer.h"

#include <glad/glad.h>

namespace Engine
{
void RendererCommand::SetClearColor(const glm::vec4 &color) { glClearColor(color.x, color.y, color.z, color.a); }

void RendererCommand::Clear() { glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); }

struct Renderer3DData
{
    Camera Camera{};
    Light Light{LightType::Directional};
};
static Renderer3DData s_Renderer3DData;

void Renderer3D::BeginScene() {}
void Renderer3D::BeginScene(const Camera &camera) { s_Renderer3DData.Camera = camera; }
void Renderer3D::BeginScene(const Camera &camera, const Light &light)
{
    BeginScene(camera);
    s_Renderer3DData.Light = light;
}

void Renderer3D::EndScene()
{
    s_Renderer3DData.Camera = Camera{};
    s_Renderer3DData.Light = Light{LightType::Directional};
}

void Renderer3D::DrawModel(Model &model, Shader &shader)
{
    //
    model.Draw(shader, s_Renderer3DData.Camera, s_Renderer3DData.Light);
}
void Renderer3D::DrawModel(Model &model, Shader &shader, const glm::vec3 &position, const glm::vec3 &rotation,
                           const glm::vec3 &scale)
{
    model.SetPosition(position);
    model.SetRotation(rotation);
    model.SetScale(scale);

    model.Draw(shader, s_Renderer3DData.Camera, s_Renderer3DData.Light);
}
} // namespace Engine