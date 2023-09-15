#include "SkyLight.h"

#include <glad/glad.h>

#include "Shader.h"
#include "InputManager.h"
#include "AssetManager.h"
#include "ShaderManager.h"
#include "TextureHDRI.h"

namespace Engine
{
// TODO: Allow blurring of cubemap
void SkyLight::Init(AssetHandle handle, const std::size_t resolution)
{
    m_EnvironmentHandle = handle;
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

    SetupCube();

    TextureHDRIRef hdrTexture = AssetManager::GetAsset<TextureHDRI>(handle);
    if (hdrTexture == nullptr) return;

    m_Shaders["equirectangularToCubemap"] =
        std::make_shared<Shader>("Resources/shaders/cubemap.vert", "Resources/shaders/cubemapConverter.frag");
    m_Shaders["cubemap"] =
        std::make_shared<Shader>("Resources/shaders/cubemapBg.vert", "Resources/shaders/cubemapBg.frag");
    m_Shaders["irradiance"] =
        std::make_shared<Shader>("Resources/shaders/cubemap.vert", "Resources/shaders/irradianceConvolution.frag");
    m_Shaders["prefilter"] =
        std::make_shared<Shader>("Resources/shaders/cubemap.vert", "Resources/shaders/prefilter.frag");
    m_Shaders["brdf"] = std::make_shared<Shader>("Resources/shaders/brdf.vert", "Resources/shaders/brdf.frag");

    m_Shaders["cubemap"]->SetUniform1i("environmentMap", 0);

    auto equirectangularToCubemapShader = m_Shaders["equirectangularToCubemap"];
    auto irradianceShader = m_Shaders["irradiance"];
    auto prefilterShader = m_Shaders["prefilter"];
    auto brdfShader = m_Shaders["brdf"];

    unsigned int captureFBO, captureRBO;
    glGenFramebuffers(1, &captureFBO);
    glGenRenderbuffers(1, &captureRBO);

    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, resolution, resolution);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);

    glGenTextures(1, &m_EnvCubemap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_EnvCubemap);
    for (unsigned int i = 0; i < 6; ++i)
    {
        // note that we store each face with 16 bit floating point values
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, resolution, resolution, 0, GL_RGB, GL_FLOAT,
                     nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
    glm::mat4 captureViews[] = {
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f))};

    // convert HDR equirectangular environment map to cubemap equivalent
    equirectangularToCubemapShader->Bind();
    equirectangularToCubemapShader->SetUniform1i("equirectangularMap", 0);
    equirectangularToCubemapShader->SetUniformMatrix4fv("projection", captureProjection);

    hdrTexture->Bind();

    glViewport(0, 0, resolution, resolution); // don't forget to configure the viewport to the capture dimensions.
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);

    for (unsigned int i = 0; i < 6; ++i)
    {
        equirectangularToCubemapShader->SetUniformMatrix4fv("view", captureViews[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, m_EnvCubemap,
                               0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        RenderCube(); // renders a 1x1 cube
    }
    auto textureId = hdrTexture->GetRendererID();
    glDeleteTextures(1, &textureId);
    equirectangularToCubemapShader->Delete();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    /*---------------------------------------------------------------------------------------------------------------------------------------------*/
    /*---------------------------------------------------------------------------------------------------------------------------------------------*/
    /*---------------------------------------------------------------------------------------------------------------------------------------------*/
    /*---------------------------------------------------------------------------------------------------------------------------------------------*/

    // Generate mipmaps from first mip face (again to reduce bright dots)
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_EnvCubemap);
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

    // create an irradiance cubemap, and re-scale capture FBO to irradiance scale.
    glGenTextures(1, &m_IrradianceMap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_IrradianceMap);
    for (unsigned int i = 0; i < 6; ++i)
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, resolution / 16, resolution / 16, 0, GL_RGB,
                     GL_FLOAT, nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, resolution / 16, resolution / 16);

    irradianceShader->Bind();
    irradianceShader->SetUniform1i("environmentMap", 0);
    irradianceShader->SetUniformMatrix4fv("projection", captureProjection);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_EnvCubemap);

    glViewport(0, 0, resolution / 16, resolution / 16);
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    for (unsigned int i = 0; i < 6; ++i)
    {
        irradianceShader->SetUniformMatrix3fv("view", captureViews[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                               m_IrradianceMap, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        RenderCube();
    }
    irradianceShader->Delete();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    /*---------------------------------------------------------------------------------------------------------------------------------------------*/
    /*---------------------------------------------------------------------------------------------------------------------------------------------*/
    /*---------------------------------------------------------------------------------------------------------------------------------------------*/
    /*---------------------------------------------------------------------------------------------------------------------------------------------*/

    glGenTextures(1, &m_PreFilterMap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_PreFilterMap);
    for (unsigned int i = 0; i < 6; ++i)
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, resolution / 4, resolution / 4, 0, GL_RGB,
                     GL_FLOAT, nullptr);
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
        const unsigned int mipWidth = (resolution / 4) * std::pow(0.5f, mipLevel);
        const unsigned int mipHeight = (resolution / 4) * std::pow(0.5f, mipLevel);
        glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipWidth, mipHeight);
        glViewport(0, 0, mipWidth, mipHeight);

        const float roughness = (float)mipLevel / (float)(maxMipLevels - 1);
        prefilterShader->SetUniform1f("roughness", roughness);
        // for 6 faces of the cube
        for (unsigned int i = 0; i < 6; ++i)
        {
            prefilterShader->SetUniformMatrix4fv("view", captureViews[i]);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                                   m_PreFilterMap, mipLevel);

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            RenderCube();
        }
    }
    prefilterShader->Delete();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    /*---------------------------------------------------------------------------------------------------------------------------------------------*/
    /*---------------------------------------------------------------------------------------------------------------------------------------------*/
    /*---------------------------------------------------------------------------------------------------------------------------------------------*/
    /*---------------------------------------------------------------------------------------------------------------------------------------------*/

    // generate 2D LUT from BRDF equations
    glGenTextures(1, &m_BrdfLUT);
    glBindTexture(GL_TEXTURE_2D, m_BrdfLUT);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, resolution, resolution, 0, GL_RG, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Reconfigure capture framebuffer object and render screen-space quad with BRDF shader
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, resolution, resolution);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_BrdfLUT, 0);

    brdfShader->Bind();
    glViewport(0, 0, resolution, resolution);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    RenderQuad();

    brdfShader->Delete();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);

    auto windowSize = InputManager::Instance().GetWindowState();
    glViewport(0, 0, windowSize.Width, windowSize.Height);
}

void SkyLight::Destroy()
{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    UnBindMaps();
}

void SkyLight::Render(const glm::mat4 &projection, const glm::mat4 &view)
{
    auto cubemap = m_Shaders["cubemap"];
    cubemap->Bind();
    cubemap->SetUniformMatrix4fv("projection", projection);
    cubemap->SetUniformMatrix4fv("view", view);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_EnvCubemap);

    RenderCube();
}

void SkyLight::BindMaps(int slot) const
{
    glActiveTexture(0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_IrradianceMap);
    glActiveTexture(1);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_PreFilterMap);
    glActiveTexture(2);
    glBindTexture(GL_TEXTURE_2D, m_BrdfLUT);
}

void SkyLight::UnBindMaps() const
{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void SkyLight::SetupCube()
{
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
    // clang-format off
    float vertices[] = {
        // back face
        -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
         1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
         1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right         
         1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
        -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
        -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left
        // front face
        -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
         1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // bottom-right
         1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
         1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
        -1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // top-left
        -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
        // left face
        -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
        -1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-left
        -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
        -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
        -1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
        -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
        // right face
         1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
         1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
         1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right         
         1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
         1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
         1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left 
        // bottom face
        -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
         1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left
         1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
         1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
        -1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
        -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
        // top face
        -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
         1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
         1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right     
         1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
        -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
        -1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // bottom-left 
    };
    // clang-format on
    m_CubeVAO.Init();
    m_CubeVAO.Bind();

    m_CubeVAO.AttachBuffer(BufferType::ARRAY, sizeof(vertices), DrawMode::STATIC, vertices);
    m_CubeVAO.EnableAttribute(0, 3, 8 * sizeof(float), (void *)0);
    m_CubeVAO.EnableAttribute(1, 3, 8 * sizeof(float), (void *)(3 * sizeof(float)));
    m_CubeVAO.EnableAttribute(2, 2, 8 * sizeof(float), (void *)(6 * sizeof(float)));

    m_CubeVAO.Unbind();
}

void SkyLight::RenderCube()
{
    m_CubeVAO.Bind();
    glDrawArrays(GL_TRIANGLES, 0, 36);
    m_CubeVAO.Unbind();
}

void SkyLight::RenderQuad()
{
    float vertices[] = {-1.0f, 1.0f, 0.0f, 0.0f, 1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
                        1.0f,  1.0f, 0.0f, 1.0f, 1.0f, 1.0f,  -1.0f, 0.0f, 1.0f, 0.0f};

    if (m_QuadVAO.GetID() == 0)
    {
        m_QuadVAO.Init();
        m_QuadVAO.Bind();

        m_QuadVAO.AttachBuffer(BufferType::ARRAY, sizeof(vertices), DrawMode::STATIC, vertices);
        m_QuadVAO.EnableAttribute(0, 3, 5 * sizeof(float), (void *)0);
        m_QuadVAO.EnableAttribute(1, 2, 5 * sizeof(float), (void *)(3 * sizeof(float)));
    }

    m_QuadVAO.Bind();
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    m_QuadVAO.Unbind();
}
} // namespace Engine
