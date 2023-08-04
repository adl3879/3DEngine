#include "RenderSystem.h"

#include <glad/glad.h>

#include "Log.h"
#include "Components.h"
#include "Shader.h"
#include "ResourceManager.h"
#include "InputManager.h"
#include "OutlineSystem.h"
#include "AssetManager.h"

namespace Engine
{
void RenderSystem::Init()
{
#ifndef NDEBUG
    LOG_CORE_INFO("OpenGL Version: {0}", (const char *)glGetString(GL_VERSION));
    LOG_CORE_INFO("GLSL Version: {0}", (const char *)glGetString(GL_SHADING_LANGUAGE_VERSION));
    LOG_CORE_INFO("OpenGL Driver Vendor: {0}", (const char *)glGetString(GL_VENDOR));
    LOG_CORE_INFO("OpenGL Renderer: {0}", (const char *)glGetString(GL_RENDERER));
#endif

    m_Shaders["modelShader"] =
        std::make_shared<Shader>("/Resources/shaders/model.vert", "/Resources/shaders/model.frag");
    m_Shaders["quadShader"] =
        std::make_shared<Shader>("/Resources/shaders/light.vert", "/Resources/shaders/light.frag");
    m_Shaders["lineShader"] = std::make_shared<Shader>("/Resources/shaders/line.vert", "/Resources/shaders/line.frag");
    m_Shaders["outlineShader"] =
        std::make_shared<Shader>("/Resources/shaders/outline.vert", "/Resources/shaders/outline.frag");
    m_Shaders["pbrShader"] = std::make_shared<Shader>("/Resources/shaders/PBR.vert", "/Resources/shaders/PBR.frag");

    auto outlineShader = m_Shaders.at("outlineShader");
    outlineShader->SetUniform1i("selectionMask", 0);

    SetupScreenQuad();
    SetupTextureSamplers();

    m_SkyLight = std::make_shared<SkyLight>();
    m_SkyLight->Init("/res/textures/hdr/skyLight.hdr", 2048);

    WindowState windowState = InputManager::Instance().GetWindowState();
    auto fbSpec = FramebufferSpecification{};
    fbSpec.Attachments = {FramebufferTextureFormat::RGBA8};
    fbSpec.Width = windowState.Width;
    fbSpec.Height = windowState.Height;
    m_FBO = std::make_shared<Engine::Framebuffer>(fbSpec);

    m_Outline = std::make_shared<OutlineSystem>();
    m_Outline->Init();

    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    SetupDefaultState();
    glEnable(GL_MULTISAMPLE);
}

void RenderSystem::Update(const Camera &camera) {}

void RenderSystem::Shutdown() {}

void RenderSystem::Render(Camera &camera, Scene &scene, const bool globalWireframe)
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    RenderModelsWithTextures(camera, scene);

    glActiveTexture(GL_TEXTURE0);
    m_SkyLight->Render(camera);
}

void RenderSystem::SetupDefaultState()
{
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

    auto pbrShader = m_Shaders.at("pbrShader");
    pbrShader->SetUniform1i("irradianceMap", 0);
    pbrShader->SetUniform1i("prefilterMap", 1);
    pbrShader->SetUniform1i("brdfLUT", 2);

    pbrShader->SetUniform1i("albedoMap", 3);
    pbrShader->SetUniform1i("normalMap", 4);
    pbrShader->SetUniform1i("metallicMap", 5);
    pbrShader->SetUniform1i("roughnessMap", 6);
}

void RenderSystem::SetupScreenQuad()
{
    float vertices[] = {1.0f, -1.0f, 1.0f, 0.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 1.0f,
                        1.0f, 1.0f,  1.0f, 1.0f, 1.0f,  -1.0f, 1.0f, 0.0f, -1.0f, 1.0f, 0.0f, 1.0f};

    m_QuadVAO.Init();
    m_QuadVAO.Bind();
    m_QuadVAO.AttachBuffer(BufferType::ARRAY, sizeof(vertices), DrawMode::STATIC, vertices);

    m_QuadVAO.EnableAttribute(0, 2, 4 * sizeof(float), (void *)0);
    m_QuadVAO.EnableAttribute(1, 2, 4 * sizeof(float), (void *)(2 * sizeof(float)));

    m_QuadVAO.Unbind();
}

void RenderSystem::RenderModelsWithTextures(Camera &camera, Scene &scene)
{
    // bind pre-computed IBL data
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_SkyLight->GetIrradianceMap());
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_SkyLight->GetPrefilterMap());
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, m_SkyLight->GetBrdfLUT());

    glBindSampler(m_SamplerPBRTextures, 3);
    glBindSampler(m_SamplerPBRTextures, 4);
    glBindSampler(m_SamplerPBRTextures, 5);
    glBindSampler(m_SamplerPBRTextures, 6);

    auto modelShader = m_Shaders.at("pbrShader");

    auto view = scene.GetRegistry().view<MeshComponent, TransformComponent, VisibilityComponent>();

    for (auto entity : view)
    {
        auto [model, transform, visibility] = view.get<MeshComponent, TransformComponent, VisibilityComponent>(entity);
        if (!visibility.IsVisible || model.Handle == 0) continue;
        const auto &entityModel = AssetManager::GetAsset<Model>(model.Handle);

        for (auto &mesh : entityModel->GetMeshes())
        {
            modelShader->Use();
            mesh.VAO.Bind();

            // entity id should be populated only once
            if (mesh.VertexSOA.EntityIDs[0] < 0.0f)
                mesh.VertexSOA.EntityIDs = std::vector<float>(mesh.VertexCount, (int)entity + 1);

            // clang-format off
            auto vertices = mesh.VertexSOA;
            mesh.VAO.SetBufferSubData(0, BufferType::ARRAY, 0, vertices.Positions.size() * sizeof(glm::vec3), &vertices.Positions[0]);
            mesh.VAO.SetBufferSubData(1, BufferType::ARRAY, 0, vertices.Normals.size() * sizeof(glm::vec3), &vertices.Normals[0]);
            mesh.VAO.SetBufferSubData(2, BufferType::ARRAY, 0, vertices.Colors.size() * sizeof(glm::vec3), &vertices.Colors[0]);
            mesh.VAO.SetBufferSubData(3, BufferType::ARRAY, 0, vertices.TexCoords.size() * sizeof(glm::vec2), &vertices.TexCoords[0]);
            mesh.VAO.SetBufferSubData(4, BufferType::ARRAY, 0, vertices.EntityIDs.size() * sizeof(float), &vertices.EntityIDs[0]);
            // clang-format on

            const auto mat = mesh.Material;
            mat->BindMaterialTextures(3);

            modelShader->SetUniformMatrix4fv("model", transform.GetTransform());
            modelShader->SetUniformMatrix3fv("normalMatrix",
                                             glm::transpose(glm::inverse(glm::mat3(transform.GetTransform()))));
            modelShader->SetUniformMatrix4fv("projectionViewMatrix", camera.GetProjectionViewMatrix());
            modelShader->SetUniform3f("cameraPosition", camera.GetPosition());

            modelShader->SetUniform3f("albedoParam", {0.0, 0.0, 0.0});
            modelShader->SetUniform1f("metallicParam", 0.5f);
            modelShader->SetUniform1f("aoParam", 1.0f);
            modelShader->SetUniform1f("roughnessParam", 0.5f);

            modelShader->SetUniform1i("numOfPointLights", 4);

            Light::SetLightUniforms(*modelShader);

            glDrawElements(GL_TRIANGLES, mesh.IndexCount, GL_UNSIGNED_INT, nullptr);
            glBindTexture(GL_TEXTURE_2D, 0);

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

void RenderSystem::RenderQuad(Camera &camera)
{
    m_QuadVAO.Bind();
    glDrawArrays(GL_TRIANGLES, 0, 6);
    m_QuadVAO.Unbind();
}
} // namespace Engine