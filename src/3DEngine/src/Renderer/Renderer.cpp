#include "Renderer.h"

#include <glad/glad.h>

#include "Model.h"
#include "Shader.h"
#include "Buffer.h"
#include "Mesh.h"
#include "Texture.h"
#include "Camera.h"

#include <memory>

namespace Engine
{
using CameraClass = Camera;

void RendererCommand::SetClearColor(const glm::vec4 &color) { glClearColor(color.x, color.y, color.z, color.a); }

void RendererCommand::Clear() { glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); }

struct Renderer3DData
{
    CameraClass Camera{};

    // model
    std::shared_ptr<Shader> ModelShader;

    // skybox
    std::shared_ptr<Shader> SkyboxShader;
    std::shared_ptr<Mesh> SkyboxMesh;
    CameraClass SkyboxCamera{};
};
static Renderer3DData s_Renderer3DData;

void Renderer3D::Init()
{
    // model
    s_Renderer3DData.ModelShader =
        std::make_unique<Engine::Shader>("/res/shaders/model.vert", "/res/shaders/lighting.frag");

    // skybox
    std::vector<float> skyboxVertices = {
        -1.0f, 1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  -1.0f, -1.0f, 1.0f,  -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, -1.0f,
        1.0f,  -1.0f, -1.0f, -1.0f, 1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  -1.0f, -1.0f, 1.0f,  -1.0f, -1.0f, 1.0f,
        1.0f,  -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, -1.0f, 1.0f,  -1.0f, 1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  -1.0f, 1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  -1.0f, 1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,  -1.0f, 1.0f,  -1.0f, -1.0f, 1.0f,  -1.0f, 1.0f,  -1.0f, 1.0f,  1.0f,  -1.0f, 1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,  1.0f,  -1.0f, 1.0f,  1.0f,  -1.0f, 1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,
        1.0f,  -1.0f, -1.0f, 1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, 1.0f};
    std::vector<unsigned int> skyboxIndices = {};
    Texture3D skyboxTexture({
        "/res/textures/skybox/right.jpg",
        "/res/textures/skybox/left.jpg",
        "/res/textures/skybox/top.jpg",
        "/res/textures/skybox/bottom.jpg",
        "/res/textures/skybox/front.jpg",
        "/res/textures/skybox/back.jpg",
    });

    s_Renderer3DData.SkyboxShader =
        std::make_shared<Engine::Shader>("/res/shaders/cubeMap.vert", "/res/shaders/cubeMap.frag");
    s_Renderer3DData.SkyboxMesh = std::make_shared<Mesh>(skyboxVertices, skyboxTexture);
}

void Renderer3D::BeginScene() {}

void Renderer3D::BeginScene(const Camera &camera) { s_Renderer3DData.Camera = camera; }

void Renderer3D::EndScene() { s_Renderer3DData.Camera = Camera{}; }

void Renderer3D::DrawModel(Model &model) { model.Draw(*s_Renderer3DData.ModelShader, s_Renderer3DData.Camera); }

void Renderer3D::DrawModel(Model &model, const glm::mat4 &transform)
{
    model.Draw(*s_Renderer3DData.ModelShader, s_Renderer3DData.Camera, transform);
}

void Renderer3D::DrawModel(Model &model, Shader &shader) { model.Draw(shader, s_Renderer3DData.Camera); }

void Renderer3D::DrawSkybox()
{
    //
    s_Renderer3DData.SkyboxMesh->DrawCubeMap(*s_Renderer3DData.SkyboxShader, s_Renderer3DData.Camera);
}
} // namespace Engine