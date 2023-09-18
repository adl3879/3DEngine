#include "SceneRenderer.h"

#include <glad/glad.h>

#include "RenderCommand.h"
#include "Components.h"
#include "InfiniteGrid.h"

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
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    auto pbrShader = ShaderManager::GetShader("Resources/shaders/PBR");
    pbrShader->SetUniform1i("irradianceMap", 0);
    pbrShader->SetUniform1i("prefilterMap", 1);
    pbrShader->SetUniform1i("brdfLUT", 2);

    m_ShadingBuffer = std::make_shared<Framebuffer>(true, glm::vec2(1280, 720));
    m_ShadingBuffer->SetTexture(std::make_shared<Texture2D>(ImageFormat::Depth), GL_DEPTH_ATTACHMENT);

	InfiniteGrid::Init();
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

void SceneRenderer::RenderScene(Scene &scene)
{
    auto pbrShader = ShaderManager::GetShader("Resources/shaders/PBR");
    pbrShader->Bind();
    pbrShader->SetUniformMatrix4fv("projectionViewMatrix", m_Projection * m_View);
    pbrShader->SetUniform3f("cameraPosition", m_CameraPosition);


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
            auto material = AssetManager::GetAsset<Material>(model.MaterialHandle);
            if (material == nullptr)
                material = AssetManager::GetAsset<Material>(entityModel->GetDefaultMaterialHandle());
            mesh.Material = material;
            if (model.ModelResource && !material)
                mesh.Material->SetMaterialParam(ParameterType::ALBEDO, glm::vec3(1, 1, 1));

            if (environment->SkyboxHDR) environment->SkyboxHDR->BindMaps();
            scene.GetLights()->SetLightUniforms(*pbrShader);

			auto trnsfrm = transform.GetTransform();
            Renderer::SubmitMesh(std::make_shared<Mesh>(mesh), trnsfrm, (int)e);
        }
    }
    Renderer::Flush(pbrShader, false);

	if (environment->SkyboxHDR != nullptr)
    {
        if (environment->CurrentSkyType == SkyType::SkyboxHDR)
        {
            scene.GetEnvironment()->SkyboxHDR->BindMaps();
            environment->SkyboxHDR->Render(m_Projection, m_View);
        }
        else scene.GetEnvironment()->SkyboxHDR->Destroy();
    }

	if (!scene.IsPlaying()) InfiniteGrid::Draw(m_Projection, m_View);
}

void SceneRenderer::ShadowPass(Scene &scene) {}
} // namespace Engine