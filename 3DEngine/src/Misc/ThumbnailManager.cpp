#include "ThumbnailManager.h"

#include <glad/glad.h>

#include "ShaderManager.h"
#include "AssetManager.h"
#include "Material.h"
#include "Renderer.h"
#include "RenderCommand.h"

namespace Engine
{
ThumbnailManager::ThumbnailManager()
{
    m_Framebuffer = std::make_shared<Framebuffer>(false, glm::vec2(1280, 720));
	//m_Framebuffer->SetTexture(std::make_shared<Texture2D>(ImageFormat::Depth), GL_DEPTH_ATTACHMENT);
    m_Framebuffer->SetTexture(std::make_shared<Texture2D>(ImageFormat::RGB16), GL_COLOR_ATTACHMENT0);
}

ThumbnailManager &ThumbnailManager::Get()
{
    static ThumbnailManager instance;
	return instance;
}

bool ThumbnailManager::IsThumbnailLoaded(const std::filesystem::path &path) const
{
	if (m_Thumbnails.find(path) != m_Thumbnails.end()) return true;
}

Texture2DRef ThumbnailManager::GetThumbnail(const std::filesystem::path &path) 
{
    m_Framebuffer->QueueResize({1280, 720});
    if (IsThumbnailLoaded(path))
    {
        m_Thumbnails[path]->Bind();
        return m_Thumbnails[path];
    }

	Texture2DRef thumbnail = std::make_shared<Texture2D>(ImageFormat::RGB16, 1280, 720);
    //thumbnail->Resize({1280, 720});
	GenerateThumbnail(path, thumbnail);
	m_Thumbnails[path] = thumbnail;
	return thumbnail;
}

void ThumbnailManager::MarkThumbnailAsDirty(const std::filesystem::path &path) 
{
	if (IsThumbnailLoaded(path)) m_Thumbnails.erase(path);
}

Texture2DRef ThumbnailManager::GenerateThumbnail(const std::filesystem::path &path, Texture2DRef texture)
{
	m_Framebuffer->SetTexture(texture);
	m_Framebuffer->Bind();
	{
	    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
		m_Framebuffer->Clear();
		//RenderCommand::Clear();
		
		RenderCommand::Disable(RendererEnum::BLEND);
		RenderCommand::Disable(RendererEnum::CULL_FACE);
  
		auto projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f);
		auto view = glm::lookAt(glm::vec3(0.0f, 0.0f, 1.0f),
											glm::vec3(0.0f, 0.0f, 0.0f),
											glm::vec3(0.0f, 1.0f, 0.0f));
		auto projectionView = projection * view;
		auto model = glm::mat4(1.0f);
		// scale
		model = glm::scale(model, glm::vec3(1.8f));

		auto pbrShader = ShaderManager::GetShader("Resources/shaders/PBR");
		pbrShader->Bind();

		// light
		pbrShader->SetUniform3f("gDirectionalLight.Color", glm::vec3(40.0f, 40.0f, 40.0f));
		pbrShader->SetUniform3f("gDirectionalLight.Direction", glm::vec3(0.6f, 0.6f, 0.6f));

		pbrShader->SetUniformMatrix4fv("model", model);
		pbrShader->SetUniformMatrix4fv("projectionViewMatrix", projectionView);
		pbrShader->SetUniform3f("cameraPosition", glm::vec3(0.0f));

		auto mat = AssetManager::GetAsset<Material>(path);
		pbrShader->SetUniform1i("hasAlbedoMap", mat->HasMaterialMap(ParameterType::ALBEDO));
		pbrShader->SetUniform1i("hasNormalMap", mat->HasMaterialMap(ParameterType::NORMAL));
		pbrShader->SetUniform1i("hasMetallicMap", mat->HasMaterialMap(ParameterType::METALLIC));
		pbrShader->SetUniform1i("hasRoughnessMap", mat->HasMaterialMap(ParameterType::ROUGHNESS));
		pbrShader->SetUniform1i("hasAoMap", mat->HasMaterialMap(ParameterType::AO));

		mat->Bind(pbrShader);

		// render sphere
		Renderer::SphereMesh->Draw(pbrShader, true);
	}
	m_Framebuffer->Unbind();

	return m_Framebuffer->GetTexture();
}
} // namespace Engine
