#include "RenderSystem.h"

#include <glad/glad.h>

#include "Log.h"
#include "Components.h"
#include "PhysicsComponents.h"
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

    // m_SkyLight = std::make_shared<SkyLight>();
    // m_SkyLight->Init("/res/textures/hdr/skyLight.hdr", 2048);

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
    // RenderPhysicsDebug(camera, scene);

    // glActiveTexture(GL_TEXTURE0);
    auto view = scene.GetRegistry().view<SkyLightComponent>();
    for (auto entity : view)
    {
        auto &skyLight = view.get<SkyLightComponent>(entity).Light;
        if (skyLight != nullptr)
        {
            // skyLight->BindMaps();
            skyLight->Render(camera);
        }
    }
    // m_SkyLight->Render(camera);
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
    glBindSampler(m_SamplerPBRTextures, 3);
    glBindSampler(m_SamplerPBRTextures, 4);
    glBindSampler(m_SamplerPBRTextures, 5);
    glBindSampler(m_SamplerPBRTextures, 6);

    auto modelShader = m_Shaders.at("pbrShader");

    auto view = scene.GetRegistry().view<MeshComponent, TransformComponent, VisibilityComponent>();

    for (auto entity : view)
    {
        auto [model, transform, visibility] = view.get<MeshComponent, TransformComponent, VisibilityComponent>(entity);

        if (!visibility.IsVisible) continue;
        if (model.Handle == 0 && model.ModelResource == nullptr) continue;

        const auto &entityModel =
            model.ModelResource ? model.ModelResource : AssetManager::GetAsset<Model>(model.Handle);

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

            modelShader->SetUniformMatrix4fv("model", transform.GetTransform());
            modelShader->SetUniformMatrix3fv("normalMatrix",
                                             glm::transpose(glm::inverse(glm::mat3(transform.GetTransform()))));
            modelShader->SetUniformMatrix4fv("projectionViewMatrix", camera.GetProjectionViewMatrix());
            modelShader->SetUniform3f("cameraPosition", camera.GetPosition());

            auto material = AssetManager::GetAsset<Material>(model.MaterialHandle);
            const auto &mat = material == nullptr ? mesh.Material : material;
            if (model.ModelResource && !material) mat->SetMaterialParam(ParameterType::ALBEDO, glm::vec3(1, 1, 1));
            mat->BindMaterialTextures(3);

            modelShader->SetUniform3f("albedoParam", mat->GetMaterialData().Albedo);
            modelShader->SetUniform1f("metallicParam", mat->GetMaterialData().Metallic);
            modelShader->SetUniform1f("aoParam", 1.0);
            modelShader->SetUniform1f("roughnessParam", mat->GetMaterialData().Roughness);

            modelShader->SetUniform1i("numOfPointLights", 4);

            Light::SetLightUniforms(*modelShader);

            glDrawElements(GL_TRIANGLES, mesh.IndexCount, GL_UNSIGNED_INT, nullptr);

            mat->UnbindMaterialTextures();
            mesh.VAO.Unbind();
        }
    }

    glBindSampler(m_SamplerPBRTextures, 0);
}

void RenderSystem::RenderModelsWithNoTextures(Camera &camera, Scene &scene) const {}

void RenderSystem::RenderPhysicsDebug(Camera &camera, Scene &scene) const
{
    auto modelShader = m_Shaders.at("modelShader");
    auto selectedEntity = scene.GetSelectedEntity();
    if (selectedEntity == entt::null) return;
    Entity ent = Entity{selectedEntity, &scene};

    auto model = ent.GetComponent<MeshComponent>();
    auto transform = ent.GetComponent<TransformComponent>();

    if (model.Handle == 0 && model.ModelResource == nullptr) return;
    if (ent.HasComponent<RigidBodyComponent>())
    {
        const auto &entityModel =
            model.ModelResource ? model.ModelResource : AssetManager::GetAsset<Model>(model.Handle);
        for (auto &mesh : entityModel->GetMeshes())
        {
            modelShader->Use();
            mesh.VAO.Bind();

            // clang-format off
            auto vertices = mesh.VertexSOA;
            mesh.VAO.SetBufferSubData(0, BufferType::ARRAY, 0, vertices.Positions.size() * sizeof(glm::vec3), &vertices.Positions[0]);
            mesh.VAO.SetBufferSubData(1, BufferType::ARRAY, 0, vertices.Normals.size() * sizeof(glm::vec3), &vertices.Normals[0]);
            mesh.VAO.SetBufferSubData(2, BufferType::ARRAY, 0, vertices.Colors.size() * sizeof(glm::vec3), &vertices.Colors[0]);
            // clang-format on

            modelShader->SetUniformMatrix4fv("model", transform.GetTransform());
            modelShader->SetUniformMatrix4fv("projectionViewMatrix", camera.GetProjectionViewMatrix());

            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            glDrawElements(GL_TRIANGLES, mesh.IndexCount, GL_UNSIGNED_INT, nullptr);
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

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