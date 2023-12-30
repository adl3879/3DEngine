#include "ThumbnailManager.h"

#include <glad/glad.h>

#include <utility>

#include "ShaderManager.h"
#include "AssetManager.h"
#include "Material.h"
#include "Renderer.h"
#include "RenderCommand.h"

namespace Engine
{
ThumbnailManager::ThumbnailManager()
{
    m_Framebuffer = std::make_shared<Framebuffer>(false, m_ThumbnailSize);
    m_Framebuffer->SetTexture(std::make_shared<Texture2D>(ImageFormat::RGB16), GL_COLOR_ATTACHMENT0);
}

ThumbnailManager &ThumbnailManager::Get()
{
    static ThumbnailManager instance;
    return instance;
}

bool ThumbnailManager::IsThumbnailLoaded(std::filesystem::path path) const
{
    if (m_Thumbnails.find(path) != m_Thumbnails.end()) return true;
    else return false;
}

Texture2DRef ThumbnailManager::GetThumbnail(std::filesystem::path path)
{
    if (IsThumbnailLoaded(path)) return m_Thumbnails[path];

    m_Queue.push(path);
    return nullptr;
}

void ThumbnailManager::MarkThumbnailAsDirty(AssetHandle handle)
{
    const auto path = AssetManager::GetRegistry()[handle].FilePath;
    if (IsThumbnailLoaded(path)) m_Thumbnails.erase(path);
}

void ThumbnailManager::OnUpdate()
{
    while (!m_Queue.empty())
    {
        auto path = m_Queue.front();
        auto assetType = Utils::GetAssetTypeFromExtension(path);
        switch (assetType)
        {
            case AssetType::Material:
            {
                auto material = AssetManager::GetAsset<Material>(path);
                Texture2DRef thumbnail =
                    std::make_shared<Texture2D>(ImageFormat::RGB16, m_ThumbnailSize.x, m_ThumbnailSize.y);
                GenerateMaterialThumbnail(material, thumbnail);
                m_Thumbnails[path] = thumbnail;
            }
            break;
            case AssetType::Mesh:
            {
                auto mesh = AssetManager::GetAsset<Mesh>(path);
                Texture2DRef thumbnail =
                    std::make_shared<Texture2D>(ImageFormat::RGB16, m_ThumbnailSize.x, m_ThumbnailSize.y);
                GenerateMeshThumbnail(mesh, thumbnail);
                m_Thumbnails[path] = thumbnail;
            }
            break;
            default: break;
        }

        m_Queue.pop();
        break;
    }
}

void ThumbnailManager::GenerateMaterialThumbnail(MaterialRef mat, Texture2DRef texture)
{
    m_Framebuffer->SetTexture(std::move(texture));
    m_Framebuffer->Bind();
    {
        RenderCommand::SetClearColor({0.2f, 0.2f, 0.2f});
        m_Framebuffer->Clear();

        RenderCommand::Disable(RendererEnum::BLEND);
        RenderCommand::Disable(RendererEnum::CULL_FACE);

        auto projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f);
        auto view = glm::lookAt(glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        auto model = glm::mat4(1.0f);
        model = glm::scale(model, glm::vec3(1.8f));

        auto pbrShader = ShaderManager::GetShader("Resources/shaders/PBR");
        pbrShader->Bind();

        // light
        pbrShader->SetUniform3f("gDirectionalLight.Color", glm::vec3(40.0f, 40.0f, 40.0f));
        pbrShader->SetUniform3f("gDirectionalLight.Direction", glm::vec3(-0.6f, 0.6f, 1.0f));

        pbrShader->SetUniformMatrix4fv("model", model);
        pbrShader->SetUniformMatrix4fv("projectionViewMatrix", (projection * view));
        pbrShader->SetUniform3f("cameraPosition", glm::vec3(0.0f));

        pbrShader->SetUniform1i("hasAlbedoMap", mat->HasMaterialMap(ParameterType::ALBEDO));
        pbrShader->SetUniform1i("hasNormalMap", mat->HasMaterialMap(ParameterType::NORMAL) && mat->GetUseNormalMap());
        pbrShader->SetUniform1i("hasMetallicMap", mat->HasMaterialMap(ParameterType::METALLIC));
        pbrShader->SetUniform1i("hasRoughnessMap", mat->HasMaterialMap(ParameterType::ROUGHNESS));
        pbrShader->SetUniform1i("hasAoMap", mat->HasMaterialMap(ParameterType::AO));

        mat->Bind(pbrShader);

        // render sphere
        Renderer::SphereMesh->Draw(pbrShader, true);
    }
    m_Framebuffer->Unbind();
}

void ThumbnailManager::GenerateMeshThumbnail(MeshRef mesh, Texture2DRef texture)
{
    m_Framebuffer->SetTexture(std::move(texture));
    m_Framebuffer->Bind();
    {
        RenderCommand::SetClearColor({0.2f, 0.2f, 0.2f});
        m_Framebuffer->Clear();

        RenderCommand::Disable(RendererEnum::BLEND);
        RenderCommand::Disable(RendererEnum::CULL_FACE);

        auto projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f);
        auto view = glm::lookAt(glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        auto model = glm::mat4(1.0f);
        model = glm::scale(model, glm::vec3(0.6f));

        const auto pbrShader = ShaderManager::GetShader("Resources/shaders/PBR");
        pbrShader->Bind();

        MaterialRef material = std::make_shared<Material>();
        material->SetMaterialParam(ParameterType::ALBEDO, glm::vec3(1.0f, 1.0f, 1.0f));
        material->Bind(pbrShader);

        // light
        pbrShader->SetUniform3f("gDirectionalLight.Color", glm::vec3(100.0f, 100.0f, 100.0f));
        pbrShader->SetUniform3f("gDirectionalLight.Direction", glm::vec3(-1.0f, 1.0f, 1.0f));

        pbrShader->SetUniformMatrix4fv("model", model);
        pbrShader->SetUniformMatrix4fv("projectionViewMatrix", (projection * view));
        pbrShader->SetUniform3f("cameraPosition", glm::vec3(0.0f));

        // render sphere
        mesh->Draw(pbrShader, true);
    }
    m_Framebuffer->Unbind();
}
} // namespace Engine
