#include "SceneRenderer.h"

#include <glad/glad.h>

#include "RenderCommand.h"
#include "Components.h"
#include "InfiniteGrid.h"
#include "Renderer.h"
#include "PostFX/Bloom.h"

namespace Engine
{
void SceneRenderer::Init()
{
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LEQUAL);
    //glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    auto pbrShader = ShaderManager::GetShader("Resources/shaders/PBR");
    pbrShader->SetUniform1i("irradianceMap", 0);
    pbrShader->SetUniform1i("prefilterMap", 1);
    pbrShader->SetUniform1i("brdfLUT", 2);

   /* m_ShadingBuffer = std::make_shared<Framebuffer>(true, glm::vec2(1280, 720));
    m_ShadingBuffer->SetTexture(std::make_shared<Texture2D>(ImageFormat::Depth), GL_DEPTH_ATTACHMENT);*/

	// hdr buffer
	m_ShadingBuffer = std::make_shared<Framebuffer>(false, glm::vec2(1280, 720));
    m_ShadingBuffer->SetTexture(std::make_shared<Texture2D>(ImageFormat::Depth), GL_DEPTH_ATTACHMENT);
	m_ShadingBuffer->SetTexture(std::make_shared<Texture2D>(ImageFormat::RGB16), GL_COLOR_ATTACHMENT0);
	m_ShadingBuffer->SetTexture(std::make_shared<Texture2D>(ImageFormat::RED_INTEGER), GL_COLOR_ATTACHMENT1);
	m_ShadingBuffer->SetTexture(std::make_shared<Texture2D>(ImageFormat::RGB16), GL_COLOR_ATTACHMENT2);

	InfiniteGrid::Init();
    Renderer::Init();
}

void SceneRenderer::Cleanup() {}

void SceneRenderer::BeginRenderScene(const glm::mat4 &projection, const glm::mat4 &view,
                                     const glm::vec3 &cameraPosition)
{
    m_Projection = projection;
    m_View = view;
    m_CameraPosition = cameraPosition;

    RenderCommand::SetClearColor({0.0f, 0.0f, 0.0f});
    RenderCommand::Clear();
}

void SceneRenderer::RenderScene(Scene &scene, Framebuffer &framebuffer)
{
    auto environment = scene.GetEnvironment();

	m_ShadingBuffer->Bind();
	
	EnvironmentPass(scene);

    auto pbrShader = ShaderManager::GetShader("Resources/shaders/PBR");
    pbrShader->Bind();
    pbrShader->SetUniformMatrix4fv("projectionViewMatrix", m_Projection * m_View);
    pbrShader->SetUniform3f("cameraPosition", m_CameraPosition);
    scene.GetLights()->SetLightUniforms(*pbrShader);

	auto view = scene.GetRegistry().view<MeshComponent, TransformComponent, VisibilityComponent>();
    for (auto &e : view)
    {
        auto [model, transform, visibility] = view.get<MeshComponent, TransformComponent, VisibilityComponent>(e);

        if (!visibility.IsVisible) continue;
        if (model.Handle == 0 && model.ModelResource == nullptr) continue;

        const auto &entityModel =
            model.ModelResource ? model.ModelResource : AssetManager::GetAsset<Model>(model.Handle);

        for (auto &mesh : entityModel->GetMeshes())
        {
            if (environment->SkyboxHDR) environment->SkyboxHDR->BindMaps();

			auto trnsfrm = transform.GetTransform();
            Renderer::SubmitMesh(std::make_shared<Mesh>(mesh), trnsfrm, (int)e);
        }
    }

    Renderer::Flush(pbrShader, false);
	if (!scene.IsPlaying()) InfiniteGrid::Draw(m_Projection, m_View, m_CameraPosition);

	auto mouse = scene.GetViewportMousePos();
    int pixel = m_ShadingBuffer->ReadPixel(1, mouse);
    scene.SetHoveredEntity((entt::entity)(pixel - 1));

	m_ShadingBuffer->Unbind();

	m_ShadingBuffer->QueueResize(framebuffer.GetSize());

	Texture2DRef finalOutput = m_ShadingBuffer->GetTexture(GL_COLOR_ATTACHMENT0);
	environment->Bloom->RenderBloomTexture(finalOutput->GetRendererID(), 0.005);

	framebuffer.Bind();
	
	auto quadShader = ShaderManager::GetShader("Resources/shaders/quad");
    quadShader->Bind();
    quadShader->SetUniform1i("scene", 0);
    quadShader->SetUniform1i("bloomBlur", 1);

    quadShader->SetUniform1f("bloomStrength", environment->BloomIntensity);
    quadShader->SetUniform1f("exposure", environment->Exposure);
	quadShader->SetUniform1i("bloomEnabled", environment->BloomEnabled);

	finalOutput->Bind(0);
	glBindTextureUnit(1, environment->Bloom->BloomTexture());

    Renderer::DrawQuad();

	framebuffer.Unbind();
}

void SceneRenderer::ShadowPass(Scene &scene) {}

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
	else
		scene.GetEnvironment()->SkyboxHDR->Destroy();
    }
}
} // namespace Engine
