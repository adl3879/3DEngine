#include "SceneRenderer.h"

#include <glad/glad.h>

#include "RenderCommand.h"
#include "Components.h"

namespace Engine
{
void SceneRenderer::Init()
{
    m_GBuffer = std::make_unique<Framebuffer>(false, glm::vec2(1280, 900));
    m_GBuffer->SetTexture(std::make_shared<Texture2D>(ImageFormat::Depth), GL_DEPTH_ATTACHMENT);
    m_GBuffer->SetTexture(std::make_shared<Texture2D>(ImageFormat::RGB8), GL_COLOR_ATTACHMENT0);
    m_GBuffer->SetTexture(std::make_shared<Texture2D>(ImageFormat::RGB8), GL_COLOR_ATTACHMENT1);
    m_GBuffer->SetTexture(std::make_shared<Texture2D>(ImageFormat::RGB8), GL_COLOR_ATTACHMENT2);
    m_GBuffer->SetTexture(std::make_shared<Texture2D>(ImageFormat::RED_INTEGER), GL_COLOR_ATTACHMENT3);

    m_ShadingBuffer = std::make_unique<Framebuffer>(true, glm::vec2(1280, 900));
    m_ShadingBuffer->SetTexture(std::make_shared<Texture2D>(ImageFormat::RGB16), GL_COLOR_ATTACHMENT0);
}

void SceneRenderer::Cleanup() {}

void SceneRenderer::BeginRenderScene(const glm::mat4 &projection, const glm::mat4 &view,
                                     const glm::vec3 &cameraPosition)
{
    m_Projection = projection;
    m_View = view;
    m_CameraPosition = cameraPosition;
}

void SceneRenderer::RenderScene(Scene &scene, Framebuffer &framebuffer) {}

void SceneRenderer::GBufferPass(Scene &scene)
{
    m_GBuffer->Bind();
    m_GBuffer->Clear();
    {
        // init
        RenderCommand::Enable(RendererEnum::FACE_CULL);
        Shader *gBufferShader = ShaderManager::GetShader("Resources/shaders/gbuffer");

        gBufferShader->Bind();
        gBufferShader->SetUniformMatrix4fv("u_Projection", m_Projection);
        gBufferShader->SetUniformMatrix4fv("u_View", m_View);

        // models
        auto view = scene.GetRegistry().view<TransformComponent, MeshComponent, VisibilityComponent>();
        for (auto e : view)
        {
            auto [transform, model, visibility] = view.get<TransformComponent, MeshComponent, VisibilityComponent>(e);
            if (!visibility.IsVisible) continue;
            if (model.Handle == 0 && model.ModelResource == nullptr) continue;

            const auto &entityModel =
                model.ModelResource ? model.ModelResource : AssetManager::GetAsset<Model>(model.Handle);
            for (auto &mesh : entityModel->GetMeshes())
            {
                Renderer::SubmitMesh(std::make_shared<Mesh>(mesh), transform.GetTransform(), (uint32_t)e);
            }
        }
        glCullFace(GL_FRONT);
        Renderer::Flush(gBufferShader, false);
    }
}

void SceneRenderer::ShadingPass(Scene &scene)
{
    m_ShadingBuffer->Bind();
    m_ShadingBuffer->Clear();
    {
        RenderCommand::Disable(RendererEnum::DEPTH_TEST);
        RenderCommand::Enable(RendererEnum::FACE_CULL);

        Shader *shadingShader = ShaderManager::GetShader("resources/Shaders/deferred");
        shadingShader->Bind();
        shadingShader->SetUniformMatrix4fv("u_Projection", m_Projection);
        shadingShader->SetUniformMatrix4fv("u_View", m_View);
        shadingShader->SetUniform3f("u_EyePosition", m_CameraPosition);

        // TODO: register light

        m_GBuffer->GetTexture(GL_DEPTH_ATTACHMENT)->Bind(5);
        m_GBuffer->GetTexture(GL_COLOR_ATTACHMENT0)->Bind(6);
        m_GBuffer->GetTexture(GL_COLOR_ATTACHMENT1)->Bind(7);
        m_GBuffer->GetTexture(GL_COLOR_ATTACHMENT2)->Bind(8);

        // TODO: register materials
        shadingShader->SetUniform1i("m_Depth", 5);
        shadingShader->SetUniform1i("m_Albedo", 6);
        shadingShader->SetUniform1i("m_Normal", 7);
        shadingShader->SetUniform1i("m_Material", 8);

        RenderCommand::Disable(RendererEnum::FACE_CULL);

        Renderer::DrawQuad();
    }
}
} // namespace Engine