#include "SkyLight.h"

#include <glad/glad.h>

#include "Shader.h"
#include "InputManager.h"
#include "AssetManager.h"
#include "ShaderManager.h"
#include "TextureHDRI.h"
#include "HDRIImporter.h"
#include "Renderer.h"

namespace Engine
{
glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
glm::mat4 captureViews[] = {
    glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
    glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
    glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)),
    glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)),
    glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
    glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f))};

SkyLight::SkyLight(const std::filesystem::path &hdrPath)
    : m_HdrPath(hdrPath)
{
    m_Shaders["equirectangularToCubemap"] = std::make_shared<Shader>("Resources/shaders/cubemap.vert", "Resources/shaders/cubemapConverter.frag");
    m_Shaders["cubemap"] = std::make_shared<Shader>("Resources/shaders/cubemapBg.vert", "Resources/shaders/cubemapBg.frag");
    m_Shaders["irradiance"] = std::make_shared<Shader>("Resources/shaders/cubemap.vert", "Resources/shaders/irradianceConvolution.frag");
    m_Shaders["prefilter"] = std::make_shared<Shader>("Resources/shaders/cubemap.vert", "Resources/shaders/prefilter.frag");
    m_Shaders["brdf"] = std::make_shared<Shader>("Resources/shaders/brdf.vert", "Resources/shaders/brdf.frag");
}

// TODO: Allow blurring of cubemap
void SkyLight::Init(const std::size_t resolution)
{
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

    CreateHDRCubemap();
    CreateConvulatedCubemap();
	CreateSpecularCubemaps();
    CreateBRDLUT();

    /*auto windowSize = InputManager::Get().GetWindowState();
    glViewport(0, 0, windowSize.Width, windowSize.Height);*/
}

void SkyLight::Render(const glm::mat4 &projection, const glm::mat4 &view)
{
    auto cubemap = m_Shaders["cubemap"];
    cubemap->Bind();
    cubemap->SetUniform1i("environmentMap", 0);
    cubemap->SetUniformMatrix4fv("projection", projection);
    cubemap->SetUniformMatrix4fv("view", view);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_EnvCubemap);

    Renderer::DrawCube();
}

void SkyLight::Bind(Shader *shader) const
{
    shader->Bind();
    shader->SetUniform1i("irradianceMap", 0);
    shader->SetUniform1i("prefilterMap", 1);
    shader->SetUniform1i("brdfLUT", 2);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_IrradianceMap);
    glActiveTexture(GL_TEXTURE0 + 1);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_PreFilterMap);
    glActiveTexture(GL_TEXTURE0 + 2);
    glBindTexture(GL_TEXTURE_2D, m_BrdfLUT);
}

void SkyLight::RenderQuad()
{
    float vertices[] = {-1.0f, 1.0f, 0.0f, 0.0f, 1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
                        1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f};

    if (m_QuadVAO.GetID() == 0)
    {
        m_QuadVAO.Init();
        m_QuadVAO.Bind();

        m_QuadVAO.AttachBuffer(BufferType::ARRAY, sizeof(vertices), DrawMode::STATIC, vertices);
        m_QuadVAO.EnableAttribute(0, 3, 5 * sizeof(float), nullptr);
        m_QuadVAO.EnableAttribute(1, 2, 5 * sizeof(float), reinterpret_cast<void *>(3 * sizeof(float)));
    }

    m_QuadVAO.Bind();
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    m_QuadVAO.Unbind();
}

void SkyLight::CreateHDRCubemap()
{
    TextureHDRIRef hdrTexture = HDRIImporter::LoadHDRI(m_HdrPath);
    if (hdrTexture == nullptr) return;

    const ShaderRef equirectangularToCubemapShader = m_Shaders["equirectangularToCubemap"];

    unsigned int captureFBO, captureRBO;
    glGenFramebuffers(1, &captureFBO);
    glGenRenderbuffers(1, &captureRBO);

    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 1024, 1024);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);

    glGenTextures(1, &m_EnvCubemap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_EnvCubemap);
    for (unsigned int i = 0; i < 6; ++i)
    {
        // note that we store each face with 16 bit floating point values
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 1024, 1024, 0, GL_RGB, GL_FLOAT,
                     nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    hdrTexture->Bind();

    // convert HDR equirectangular environment map to cubemap equivalent
    equirectangularToCubemapShader->Bind();
    equirectangularToCubemapShader->SetUniform1i("equirectangularMap", 0);
    equirectangularToCubemapShader->SetUniformMatrix4fv("projection", captureProjection);

    glViewport(0, 0, 1024, 1024); // don't forget to configure the viewport to the capture dimensions.
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    for (unsigned int i = 0; i < 6; ++i)
    {
        equirectangularToCubemapShader->SetUniformMatrix4fv("view", captureViews[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, m_EnvCubemap,
                               0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        Renderer::DrawCube();
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Generate mipmaps from first mip face (again to reduce bright dots)
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_EnvCubemap);
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
}

void SkyLight::CreateConvulatedCubemap()
{
    TextureHDRIRef hdrTexture = HDRIImporter::LoadHDRI(m_HdrPath);
    if (hdrTexture == nullptr) return;

    m_Shaders["cubemap"]->SetUniform1i("environmentMap", 0);

    const ShaderRef irradianceShader = m_Shaders["irradiance"];

    unsigned int captureFBO, captureRBO;
    glGenFramebuffers(1, &captureFBO);
    glGenRenderbuffers(1, &captureRBO);

    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 32, 32);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);

    glGenTextures(1, &m_IrradianceMap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_IrradianceMap);
    for (unsigned int i = 0; i < 6; ++i)
    {
        // note that we store each face with 16 bit floating point values
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 32, 32, 0, GL_RGB, GL_FLOAT, nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_EnvCubemap);

    // convert HDR equirectangular environment map to cubemap equivalent
    irradianceShader->Bind();
    irradianceShader->SetUniform1i("environmentMap", 0);
    irradianceShader->SetUniformMatrix4fv("projection", captureProjection);

    glViewport(0, 0, 32, 32); // don't forget to configure the viewport to the capture dimensions.
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    for (unsigned int i = 0; i < 6; ++i)
    {
        irradianceShader->SetUniformMatrix4fv("view", captureViews[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, m_IrradianceMap,
                               0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        Renderer::DrawCube();
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void SkyLight::CreateSpecularCubemaps()
{
    const ShaderRef prefilterShader = m_Shaders["prefilter"];

	unsigned int captureFBO, captureRBO;
    glGenFramebuffers(1, &captureFBO);
    glGenRenderbuffers(1, &captureRBO);

	glGenTextures(1, &m_PreFilterMap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_PreFilterMap);
    for (unsigned int i = 0; i < 6; ++i)
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 1024, 1024, 0, GL_RGB, GL_FLOAT, nullptr);
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

    prefilterShader->Bind();
    prefilterShader->SetUniform1i("environmentMap", 0);
    prefilterShader->SetUniformMatrix4fv("projection", captureProjection);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_EnvCubemap);

    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    const unsigned int maxMipLevels = 5;
    for (unsigned int mipLevel = 0; mipLevel < maxMipLevels; ++mipLevel)
    {
        // reisze framebuffer according to mip-level size.
        uint32_t mipWidth = 1024 * std::pow(0.5f, mipLevel);
        uint32_t mipHeight = 1024 * std::pow(0.5f, mipLevel);
        glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipWidth, mipHeight);
        glViewport(0, 0, mipWidth, mipHeight);

        const float roughness = (float)mipLevel / (float)(maxMipLevels - 1);
        prefilterShader->SetUniform1f("roughness", roughness);
        // for 6 faces of the cube
        for (unsigned int i = 0; i < 6; ++i)
        {
            prefilterShader->SetUniformMatrix4fv("view", captureViews[i]);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, m_PreFilterMap, mipLevel);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            Renderer::DrawCube();
        }
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void SkyLight::CreateBRDLUT()
{ 
	const ShaderRef brdfShader = m_Shaders["brdf"];

    unsigned int captureFBO, captureRBO;
    glGenFramebuffers(1, &captureFBO);
    glGenRenderbuffers(1, &captureRBO);

    // generate 2D LUT from BRDF equations
    glGenTextures(1, &m_BrdfLUT);
    glBindTexture(GL_TEXTURE_2D, m_BrdfLUT);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, 512, 512, 0, GL_RG, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Reconfigure capture framebuffer object and render screen-space quad with BRDF shader
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_BrdfLUT, 0);

    brdfShader->Bind();
    glViewport(0, 0, 512, 512);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    RenderQuad();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
} // namespace Engine
