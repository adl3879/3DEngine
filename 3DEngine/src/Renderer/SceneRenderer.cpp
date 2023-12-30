#include "SceneRenderer.h"

#include <glad/glad.h>

#include "RenderCommand.h"
#include "Components.h"
#include "InfiniteGrid.h"
#include "Renderer.h"
#include "PostFX/Bloom.h"
#include "TextureImporter.h"
#include "PhysicsComponents.h"
#include "PhysicsManager.h"

namespace Engine
{
Texture2DRef cameraSprite = nullptr;

void SceneRenderer::Init()
{
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LEQUAL);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    auto pbrShader = ShaderManager::GetShader("Resources/shaders/PBR");
    pbrShader->SetUniform1i("irradianceMap", 0);
    pbrShader->SetUniform1i("prefilterMap", 1);
    pbrShader->SetUniform1i("brdfLUT", 2);

    // hdr buffer
    m_ShadingBuffer = std::make_shared<Framebuffer>(false, glm::vec2(1280, 720));
    m_ShadingBuffer->SetTexture(std::make_shared<Texture2D>(ImageFormat::Depth), GL_DEPTH_ATTACHMENT);
    m_ShadingBuffer->SetTexture(std::make_shared<Texture2D>(ImageFormat::RGB16), GL_COLOR_ATTACHMENT0);
    m_ShadingBuffer->SetTexture(std::make_shared<Texture2D>(ImageFormat::RED_INTEGER), GL_COLOR_ATTACHMENT1);

    // outline
    m_OutlineBuffer = std::make_shared<Framebuffer>(false, glm::vec2(1280, 720));
    m_OutlineBuffer->SetTexture(std::make_shared<Texture2D>(ImageFormat::Depth), GL_DEPTH_ATTACHMENT);
    m_OutlineBuffer->SetTexture(std::make_shared<Texture2D>(ImageFormat::RGBA8), GL_COLOR_ATTACHMENT0);

    m_Edge = std::make_shared<Framebuffer>(false, glm::vec2(1280, 720));
    m_Edge->SetTexture(std::make_shared<Texture2D>(ImageFormat::Depth), GL_DEPTH_ATTACHMENT);
    m_Edge->SetTexture(std::make_shared<Texture2D>(ImageFormat::RGBA8), GL_COLOR_ATTACHMENT0);

    // shadow
    m_ShadowBuffer = std::make_shared<Framebuffer>(false, glm::vec2(2048, 2048));
    m_ShadowBuffer->SetTexture(std::make_shared<Texture2D>(ImageFormat::Depth), GL_DEPTH_ATTACHMENT);

    InfiniteGrid::Init();
    Renderer::Init();

    cameraSprite = TextureImporter::LoadTexture2D("Resources/Textures/camera.png");
}

void SceneRenderer::Cleanup()
{
}

void SceneRenderer::BeginRenderScene(const glm::mat4 &projection, const glm::mat4 &view,
                                     const glm::vec3 &cameraPosition)
{
    m_Projection = projection;
    m_View = view;
    m_CameraPosition = cameraPosition;

    RenderCommand::SetClearColor({0.0f, 0.0f, 0.0f});
    RenderCommand::Clear();
}

float near_plane = 1.0f, far_plane = 7.5f;
glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
glm::mat4 lightView =
    glm::lookAt(glm::vec3(-2.0f, 4.0f, -1.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
glm::mat4 lightSpaceMatrix = lightProjection * lightView;

void SceneRenderer::RenderScene(Scene &scene, Framebuffer &framebuffer)
{
    const auto environment = scene.GetEnvironment();

    // ShadowPass(scene);

    auto view = scene.GetRegistry().view<MeshComponent, TransformComponent, VisibilityComponent>();
    // shading
    {
        m_ShadingBuffer->Bind();
        m_ShadingBuffer->Clear();

        EnvironmentPass(scene);

        auto pbrShader = ShaderManager::GetShader("Resources/shaders/PBR");
        pbrShader->Bind();
        pbrShader->SetUniformMatrix4fv("projectionViewMatrix", m_Projection * m_View);
        pbrShader->SetUniform3f("cameraPosition", m_CameraPosition);
        scene.GetLights()->SetLightUniforms(*pbrShader);
        pbrShader->SetUniformMatrix4fv("lightSpaceMatrix", lightSpaceMatrix);

        pbrShader->SetUniform1i("shadowMap", 8);
        m_ShadowBuffer->GetTexture(GL_DEPTH_ATTACHMENT)->Bind(8);

        for (auto &e : view)
        {
            auto [mesh, transform, visibility] = view.get<MeshComponent, TransformComponent, VisibilityComponent>(e);

            if (!visibility.IsVisible) continue;
            if (mesh.Handle == 0 && mesh.ModelResource == nullptr) continue;

            const auto &m = AssetManager::GetAsset<Mesh>(mesh.Handle);

            if (environment->SkyboxHDR) environment->SkyboxHDR->BindMaps();

            auto trnsfrm = transform.GetTransform();
            const bool isHandleValid = AssetManager::IsAssetHandleValid(mesh.MaterialHandle);
            const MaterialRef mat =
                AssetManager::GetAsset<Material>(isHandleValid ? mesh.MaterialHandle : m->DefaultMaterialHandle);
            Renderer::SubmitMesh(m, mat, trnsfrm, static_cast<int>(e));
        }

        Renderer::Flush(pbrShader, false);
        if (!scene.IsPlaying() && m_ShowDebug)
        {
            // block is executed only in debug mode
            auto camView = scene.GetRegistry().view<CameraComponent, TransformComponent>();
            for (auto &e : camView)
            {
                auto [camera, transform] = camView.get<CameraComponent, TransformComponent>(e);
                Renderer::DrawCameraFrustum(m_Projection, m_View, transform.GetTransform());
            }

            // physics debug
            if (scene.IsDebugDrawEnabled())
            {
                auto physxView = scene.GetRegistry().view<TransformComponent>();
                for (const auto entity : physxView)
                {
                    const auto ent = Entity{entity, &scene};
                    PhysicsManager::Get().DrawDebug(m_Projection, m_View, ent);
                }
            }

            if (scene.IsGridEnabled()) InfiniteGrid::Draw(m_Projection, m_View, m_CameraPosition);

            // weird?
            const auto mouse = scene.GetViewportMousePos();
            const int pixel = m_ShadingBuffer->ReadPixel(1, mouse);
            scene.SetHoveredEntity(static_cast<entt::entity>(pixel - 1));
        }

        m_ShadingBuffer->Unbind();
    }

    // outline
    if (!scene.IsPlaying() && m_ShowDebug)
    {
        const auto outlineShader = ShaderManager::GetShader("Resources/shaders/outline");
        outlineShader->Bind();
        outlineShader->SetUniformMatrix4fv("projectionViewMatrix", m_Projection * m_View);

        m_OutlineBuffer->Bind();
        m_OutlineBuffer->Clear();

        for (auto &e : view)
        {
            if (e != scene.GetSelectedEntity()) continue;
            auto [mesh, transform] = view.get<MeshComponent, TransformComponent>(e);

            if (mesh.Handle == 0 && mesh.ModelResource == nullptr) continue;

            const auto &m = AssetManager::GetAsset<Mesh>(mesh.Handle);

            auto trnsfrm = transform.GetTransform();
            bool isHandleValid = AssetManager::IsAssetHandleValid(mesh.MaterialHandle);
            MaterialRef mat =
                AssetManager::GetAsset<Material>(isHandleValid ? mesh.MaterialHandle : m->DefaultMaterialHandle);
            Renderer::SubmitMesh(m, mat, trnsfrm, (int)e);
        }
        Renderer::Flush(outlineShader, false);
        m_OutlineBuffer->Unbind();

        m_Edge->Bind();
        m_Edge->Clear();

        auto edgeShader = ShaderManager::GetShader("Resources/shaders/edgeDetection");
        edgeShader->Bind();
        edgeShader->SetUniform1i("mask", 0);
        // dimensions
        edgeShader->SetUniform1f("width", m_Edge->GetSize().x);
        edgeShader->SetUniform1f("height", m_Edge->GetSize().y);

        m_OutlineBuffer->GetTexture()->Bind(0);

        Renderer::DrawQuad();
        m_Edge->Unbind();
    }

    {
        Texture2DRef finalOutput = m_ShadingBuffer->GetTexture();
        environment->Bloom->RenderBloomTexture(finalOutput->GetRendererID(), 0.005);

        framebuffer.Bind();
        framebuffer.Clear();

        auto quadShader = ShaderManager::GetShader("Resources/shaders/quad");
        quadShader->Bind();
        quadShader->SetUniform1i("scene", 0);
        quadShader->SetUniform1i("bloomBlur", 1);
        quadShader->SetUniform1i("outlineTexture", 2);

        quadShader->SetUniform1f("bloomStrength", environment->BloomIntensity);
        quadShader->SetUniform1f("exposure", environment->Exposure);
        quadShader->SetUniform1i("bloomEnabled", environment->BloomEnabled);

        finalOutput->Bind(0);
        glBindTextureUnit(1, environment->Bloom->BloomTexture());
        m_Edge->GetTexture()->Bind(2);

        Renderer::DrawQuad();

        framebuffer.Unbind();
    }

    m_ShadingBuffer->QueueResize(framebuffer.GetSize());
    m_OutlineBuffer->QueueResize(framebuffer.GetSize());
    m_Edge->QueueResize(framebuffer.GetSize());
    // m_ShadowBuffer->QueueResize(framebuffer.GetSize());
}

void SceneRenderer::ShadowPass(Scene &scene)
{
}

void SceneRenderer::EnvironmentPass(Scene &scene)
{
    auto environment = scene.GetEnvironment();

    if (environment->CurrentSkyType == SkyType::ClearColor)
    {
        RenderCommand::SetClearColor(environment->AmbientColor);
        RenderCommand::Clear();
    }
    else if (environment->CurrentSkyType == SkyType::ProceduralSky)
    {
        environment->ProceduralSkybox->Draw(m_Projection, m_View);
    }
    else if (environment->SkyboxHDR != nullptr)
    {
        if (environment->CurrentSkyType == SkyType::SkyboxHDR)
        {
            scene.GetEnvironment()->SkyboxHDR->BindMaps();
            environment->SkyboxHDR->Render(m_Projection, m_View);
        }
        else scene.GetEnvironment()->SkyboxHDR->Destroy();
    }
}
} // namespace Engine
