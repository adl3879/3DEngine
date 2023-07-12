#include "RenderSystem.h"

#include <glad/glad.h>

#include "Log.h"
#include "Components.h"
#include "Shader.h"

namespace Engine
{
unsigned int VAO;

void RenderSystem::Init()
{
#ifndef NDEBUG
    LOG_CORE_INFO("OpenGL Version: {0}", (const char *)glGetString(GL_VERSION));
    LOG_CORE_INFO("GLSL Version: {0}", (const char *)glGetString(GL_SHADING_LANGUAGE_VERSION));
    LOG_CORE_INFO("OpenGL Driver Vendor: {0}", (const char *)glGetString(GL_VENDOR));
    LOG_CORE_INFO("OpenGL Renderer: {0}", (const char *)glGetString(GL_RENDERER));
#endif

    m_Shaders["modelShader"] = std::make_shared<Shader>("/res/shaders/model.vert", "/res/shaders/model.frag");
    m_Shaders["quadShader"] = std::make_shared<Shader>("/res/shaders/light.vert", "/res/shaders/light.frag");
    m_Shaders["lineShader"] = std::make_shared<Shader>("/res/shaders/line.vert", "/res/shaders/line.frag");
    m_Shaders["outlineShader"] = std::make_shared<Shader>("/res/shaders/outline.vert", "/res/shaders/outline.frag");

    SetupScreenQuad();
    SetupLine();
    SetupTextureSamplers();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_STENCIL_TEST);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    // SetupDefaultState();
    glEnable(GL_MULTISAMPLE);
}

void RenderSystem::Update(const Camera &camera) {}

void RenderSystem::Shutdown() {}

void RenderSystem::Render(Camera &camera, Scene &scene, const bool globalWireframe)
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    RenderModelsWithTextures(camera, scene);
    // RenderModelsWithNoTextures(camera, scene);
    // RenderQuad(camera);

    // RenderLine(camera, glm::vec3(0.0f), glm::vec3(2.0f), glm::vec3(1.0f, 0.2f, 0.3f));
}

void RenderSystem::SetupDefaultState()
{
    glFrontFace(GL_CCW);
    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void RenderSystem::SetupTextureSamplers()
{
    // Sampler for PBR textures used on meshes
    glGenSamplers(1, &m_SamplerPBRTextures);
    glSamplerParameteri(m_SamplerPBRTextures, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glSamplerParameteri(m_SamplerPBRTextures, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glSamplerParameteri(m_SamplerPBRTextures, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glSamplerParameteri(m_SamplerPBRTextures, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // set texture units
    // TODO: change uniform names
    auto modelShader = m_Shaders.at("modelShader");
    modelShader->SetUniform1i("diffuse0", 0);
    modelShader->SetUniform1i("normal", 1);
    modelShader->SetUniform1i("metallic", 2);
    modelShader->SetUniform1i("specular0", 3);
}

void RenderSystem::SetupScreenQuad()
{
    std::vector<Vertex> quadVerts = {
        Vertex{.Position{0.5f, 0.5f, 0.0f}},
        Vertex{.Position{0.5f, -0.5f, 0.0f}},
        Vertex{.Position{-0.5f, -0.5f, 0.0f}},
        Vertex{.Position{-0.5f, 0.5f, 0.0f}},
    };
    std::vector<uint32_t> quadIndices = {
        0, 1, 3, // first Triangle
        1, 2, 3  // second Triangle
    };

    m_QuadVAO.Init();
    m_QuadVAO.Bind();
    m_QuadVAO.AttachBuffer(BufferType::ARRAY, sizeof(Vertex) * quadVerts.size(), DrawMode::STATIC, quadVerts.data());
    m_QuadVAO.AttachBuffer(BufferType::ELEMENT, sizeof(uint32_t) * quadIndices.size(), DrawMode::STATIC,
                           quadIndices.data());
    m_QuadVAO.EnableAttribute(0, 3, sizeof(Vertex), (void *)0);
    m_QuadVAO.Unbind();
}

void RenderSystem::SetupLine()
{
    m_LineVAO.Init();
    m_LineVAO.Bind();

    m_LineVAO.AttachBuffer(BufferType::ARRAY, 6 * sizeof(float), DrawMode::DYNAMIC, nullptr);
    m_LineVAO.EnableAttribute(0, 3, 3 * sizeof(float), (void *)0);
    m_LineVAO.Unbind();
}

void RenderSystem::RenderModelsWithTextures(Camera &camera, Scene &scene) const
{
    glBindSampler(m_SamplerPBRTextures, 0);
    glBindSampler(m_SamplerPBRTextures, 1);
    glBindSampler(m_SamplerPBRTextures, 2);
    glBindSampler(m_SamplerPBRTextures, 3);

    auto modelShader = m_Shaders.at("modelShader");
    auto outlineShader = m_Shaders.at("outlineShader");

    auto view = scene.GetRegistry().view<ModelComponent, TransformComponent>();
    for (auto entity : view)
    {
        auto [model, transform] = view.get<ModelComponent, TransformComponent>(entity);
        if (model.Model == nullptr) continue;

        for (auto mesh : model.Model->GetMeshes())
        {
            modelShader->Use();
            mesh.VAO.Bind();

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, mesh.Material->GetParameterTexture(Material::ALBEDO));
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, mesh.Material->GetParameterTexture(Material::NORMAL));
            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, mesh.Material->GetParameterTexture(Material::METALLIC));
            glActiveTexture(GL_TEXTURE3);
            glBindTexture(GL_TEXTURE_2D, mesh.Material->GetParameterTexture(Material::ROUGHNESS));

            modelShader->SetUniformMatrix4fv("model", transform.GetTransform());
            modelShader->SetUniformMatrix4fv("projectionViewMatrix", camera.GetProjectionViewMatrix());
            modelShader->SetUniform3f("gCameraPos", camera.GetPosition());

            // set materials
            // TODO: rewrite shader to fix this weirdness
            modelShader->SetUniform3f("gMaterial.AmbientColor", glm::vec3{1.0f});
            modelShader->SetUniform3f("gMaterial.DiffuseColor", glm::vec3{1.0f});
            modelShader->SetUniform3f("gMaterial.SpecularColor", glm::vec3{1.0f});

            Light::SetLightUniforms(*modelShader);

            glStencilFunc(GL_ALWAYS, 1, 0xFF);
            glStencilMask(0xFF);

            glDrawElements(GL_TRIANGLES, mesh.IndexCount, GL_UNSIGNED_INT, nullptr);
            glBindTexture(GL_TEXTURE_2D, 0);

            // draws outline on selected model
            if (scene.GetSelectedEntity() == entity)
            {
                glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
                glStencilMask(0x00);
                glDisable(GL_DEPTH_TEST);

                outlineShader->Use();
                outlineShader->SetUniform1f("outlining", 0.06f);
                outlineShader->SetUniformMatrix4fv("model", transform.GetTransform());
                outlineShader->SetUniformMatrix4fv("projectionViewMatrix", camera.GetProjectionViewMatrix());
                glDrawElements(GL_TRIANGLES, mesh.IndexCount, GL_UNSIGNED_INT, nullptr);

                glStencilMask(0xFF);
                glStencilFunc(GL_ALWAYS, 1, 0xFF);
                glEnable(GL_DEPTH_TEST);
            }

            mesh.VAO.Unbind();
        }
    }

    glBindSampler(m_SamplerPBRTextures, 0);
}

void RenderSystem::RenderModelsWithNoTextures(Camera &camera, Scene &scene) const
{
    auto modelShader = m_Shaders.at("modelShader");
    auto view = scene.GetRegistry().view<ModelComponent, TransformComponent>();
    for (auto entity : view)
    {
        auto [model, transform] = view.get<ModelComponent, TransformComponent>(entity);
        if (model.Model == nullptr) continue;

        for (auto mesh : model.Model->GetMeshes())
        {
            modelShader->Use();
            mesh.VAO.Bind();

            modelShader->SetUniformMatrix4fv("model", transform.GetTransform());
            modelShader->SetUniformMatrix4fv("projectionViewMatrix", camera.GetProjectionViewMatrix());

            glDrawElements(GL_TRIANGLES, mesh.IndexCount, GL_UNSIGNED_INT, nullptr);
            mesh.VAO.Unbind();
        }
    }
}

void RenderSystem::RenderQuad(Camera &camera) const
{
    auto modelShader = m_Shaders.at("quadShader");
    modelShader->Use();

    m_QuadVAO.Bind();
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    m_QuadVAO.Unbind();
}

void RenderSystem::RenderLine(Camera &camera, const glm::vec3 &start, const glm::vec3 &end, const glm::vec3 &color)
{
    float vertices[] = {start.x, start.y, start.z, end.x, end.y, end.z};
    m_LineVAO.SetBufferSubData(BufferType::ARRAY, 0, sizeof(vertices), vertices);

    auto lineShader = m_Shaders.at("lineShader");
    lineShader->Use();
    m_LineVAO.Bind();

    lineShader->SetUniformMatrix4fv("projectionViewMatrix", camera.GetProjectionViewMatrix());
    lineShader->SetUniform3f("color", color);

    glDrawArrays(GL_LINES, 0, 2);
    m_LineVAO.Unbind();
}
} // namespace Engine