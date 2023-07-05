#include "Renderer.h"

#include <glad/glad.h>

#include "Model.h"
#include "Shader.h"
#include "Buffer.h"
#include "Mesh.h"
#include "Texture.h"
#include "Camera.h"
#include <any>

#include <memory>

namespace Engine
{
void RendererCommand::SetClearColor(const glm::vec4 &color) { glClearColor(color.x, color.y, color.z, color.a); }

void RendererCommand::Clear() { glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); }

struct Renderer3DData
{
    // camera
    std::any Camera;

    // model
    std::shared_ptr<Shader> ModelShader;

    // skybox
    std::shared_ptr<Shader> SkyboxShader;
    std::shared_ptr<Mesh> SkyboxMesh;
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

void Renderer3D::BeginScene(const PerspectiveCamera &camera) { s_Renderer3DData.Camera = camera; }

void Renderer3D::BeginScene(const EditorCamera &camera) { s_Renderer3DData.Camera = camera; }

void Renderer3D::EndScene() {}

void Renderer3D::DrawModel(Model &model)
{
    // TODO: add support for other camera types
    auto camera = std::any_cast<EditorCamera>(s_Renderer3DData.Camera);
    model.Draw(*s_Renderer3DData.ModelShader, camera);
}

void Renderer3D::DrawModel(Model &model, const glm::mat4 &transform)
{
    auto camera = std::any_cast<EditorCamera>(s_Renderer3DData.Camera);
    model.Draw(*s_Renderer3DData.ModelShader, camera, transform);
}

void Renderer3D::DrawModel(Model &model, Shader &shader)
{
    auto camera = std::any_cast<EditorCamera>(s_Renderer3DData.Camera);
    model.Draw(shader, camera);
}

void Renderer3D::DrawSkybox()
{
    auto camera = std::any_cast<EditorCamera>(s_Renderer3DData.Camera);

    s_Renderer3DData.SkyboxMesh->DrawCubeMap(*s_Renderer3DData.SkyboxShader, camera);
}
} // namespace Engine