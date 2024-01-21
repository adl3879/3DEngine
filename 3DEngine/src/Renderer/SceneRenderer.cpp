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
#include "Mesh.h"

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

    // shading
    
    m_ShadingBuffer->Bind();
    m_ShadingBuffer->Clear();

    EnvironmentPass(scene);

    auto pbrShader = ShaderManager::GetShader("Resources/shaders/PBR");
    pbrShader->Bind();
    pbrShader->SetUniformMatrix4fv("projectionViewMatrix", m_Projection * m_View);
    pbrShader->SetUniform3f("cameraPosition", m_CameraPosition);
    scene.GetLights()->SetLightUniforms(*pbrShader);
    pbrShader->SetUniformMatrix4fv("lightSpaceMatrix", lightSpaceMatrix);

    if (environment->SkyboxHDR) environment->SkyboxHDR->BindMaps();

    auto view = scene.GetRegistry().view<StaticMeshComponent, TransformComponent, VisibilityComponent>();
	// static meshes
    for (auto &e : view)
    {
        auto [mesh, transform, visibility] = view.get<StaticMeshComponent, TransformComponent, VisibilityComponent>(e);

        if (!visibility.IsVisible) continue;
        if (mesh.Handle == 0) continue;

        const auto &asset = AssetManager::GetAsset<Mesh>(mesh.Handle);
        for (const auto &m : asset->StaticMeshes)
        {
            auto trnsfrm = transform.GetTransform();
            MaterialRef mat = AssetManager::GetAsset<Material>(mesh.MaterialHandle);
            if (mat == nullptr)
            {
                mat = m.DefaultMaterial;
                mat->SetTextureFindPath(AssetManager::GetRegistry()[mesh.Handle].FilePath);
            }

            Renderer::SubmitMesh(std::make_shared<StaticMesh>(m), mat, trnsfrm, static_cast<int>(e));
        }
    }
    Renderer::Flush(pbrShader, false);

	// skinned meshes
	auto skinnedShader = ShaderManager::GetShader("Resources/shaders/skinned");
    skinnedShader->Bind();
    skinnedShader->SetUniformMatrix4fv("projectionViewMatrix", m_Projection * m_View);
    skinnedShader->SetUniform3f("cameraPosition", m_CameraPosition);
    scene.GetLights()->SetLightUniforms(*skinnedShader);
    skinnedShader->SetUniformMatrix4fv("lightSpaceMatrix", lightSpaceMatrix);

    auto skinnedView = scene.GetRegistry().view<SkinnedMeshComponent, AnimationControllerComponent, TransformComponent, VisibilityComponent>();
	for (auto& e : skinnedView)
	{
        auto [skinnedMesh, animationController, transform, visibility] = 
			skinnedView.get<SkinnedMeshComponent, AnimationControllerComponent, TransformComponent, VisibilityComponent>(e);

		if (!visibility.IsVisible) continue;
        if (skinnedMesh.Handle == 0) continue;

		const auto& asset = AssetManager::GetAsset<Mesh>(skinnedMesh.Handle);
		for (auto& m : asset->SkinnedMeshData.SkinnedMeshes)
		{
            auto trnsfrm = transform.GetTransform();
            MaterialRef mat = AssetManager::GetAsset<Material>(skinnedMesh.MaterialHandle);
            if (mat == nullptr)
            {
                mat = m.DefaultMaterial;
                mat->SetTextureFindPath(AssetManager::GetRegistry()[skinnedMesh.Handle].FilePath);
            }

			auto animator = animationController.Animator;
			auto transform = animator->GetFinalBoneMatrices();
			for (int i = 0; i < transform.size(); ++i)
                skinnedShader->SetUniformMatrix4fv("finalBonesMatrices[" + std::to_string(i) + "]", transform[i]);

			Renderer::SubmitSkinnedMesh(std::make_shared<SkinnedMesh>(m), mat, trnsfrm, static_cast<int>(e));
		}
	}
	Renderer::Flush(skinnedShader, false);

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
