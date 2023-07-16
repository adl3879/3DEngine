#include "RenderSystem.h"

#include <glad/glad.h>

#include "Log.h"
#include "Components.h"
#include "Shader.h"

namespace Engine
{
int indexCount = 0;

void RenderSystem::Init()
{
#ifndef NDEBUG
    LOG_CORE_INFO("OpenGL Version: {0}", (const char *)glGetString(GL_VERSION));
    LOG_CORE_INFO("GLSL Version: {0}", (const char *)glGetString(GL_SHADING_LANGUAGE_VERSION));
    LOG_CORE_INFO("OpenGL Driver Vendor: {0}", (const char *)glGetString(GL_VENDOR));
    LOG_CORE_INFO("OpenGL Renderer: {0}", (const char *)glGetString(GL_RENDERER));
#endif

    m_Shaders["modelShader"] = std::make_shared<Shader>("/res/shaders/model.vert", "/res/shaders/model.frag");
    m_Shaders["quadShader"] = std::make_shared<Shader>("/res/shaders/light.vert", "/res/shaders/light.frag");
    m_Shaders["lineShader"] = std::make_shared<Shader>("/res/shaders/line.vert", "/res/shaders/line.frag");
    m_Shaders["outlineShader"] = std::make_shared<Shader>("/res/shaders/outline.vert", "/res/shaders/outline.frag");
    m_Shaders["pbrShader"] = std::make_shared<Shader>("/res/shaders/PBR.vert", "/res/shaders/PBR.frag");

    SetupScreenQuad();
    SetupLine();
    SetupTextureSamplers();
    SetupSphere();

    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    // SetupDefaultState();
    glEnable(GL_MULTISAMPLE);
}

void RenderSystem::Update(const Camera &camera) {}

void RenderSystem::Shutdown() {}

glm::vec3 lightPositions[] = {
    glm::vec3(-10.0f, 10.0f, 10.0f),
    glm::vec3(10.0f, 10.0f, 10.0f),
    glm::vec3(-10.0f, -10.0f, 10.0f),
    glm::vec3(10.0f, -10.0f, 10.0f),
};
glm::vec3 lightColors[] = {glm::vec3(300.0f, 300.0f, 300.0f), glm::vec3(300.0f, 300.0f, 300.0f),
                           glm::vec3(300.0f, 300.0f, 300.0f), glm::vec3(300.0f, 300.0f, 300.0f)};

int nrRows = 7;
int nrColumns = 7;
float spacing = 2.5;

void RenderSystem::Render(Camera &camera, Scene &scene, const bool globalWireframe)
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    RenderModelsWithTextures(camera, scene);

    auto pbrShader = m_Shaders.at("pbrShader");
    pbrShader->Use();

    auto model = glm::mat4(1.0f);
    for (int row = 0; row < nrRows; ++row)
    {
        for (int col = 0; col < nrColumns; ++col)
        {
            model = glm::mat4(1.0f);
            model = glm::translate(model,
                                   glm::vec3((col - (nrColumns / 2)) * spacing, (row - (nrRows / 2)) * spacing, 0.0f));
            model = glm::scale(model, glm::vec3(0.7f));

            pbrShader->SetUniformMatrix4fv("model", model);
            pbrShader->SetUniformMatrix3fv("normalMatrix", glm::transpose(glm::inverse(glm::mat3(model))));

            RenderSphere(camera, glm::vec3(0.0f), 1.0, {1.0f, 0.0f, 0.0f});
        }
    }

    for (int i = 0; i < sizeof(lightPositions); ++i)
    {
        glm::vec3 newPos = lightPositions[i] + glm::vec3(sin(glfwGetTime() * 5.0) * 5.0, 0.0, 0.0);
        newPos = lightPositions[i];
        pbrShader->SetUniform3f("gPointLights[" + std::to_string(i) + "].Position", newPos);
        pbrShader->SetUniform3f("gPointLights[" + std::to_string(i) + "].Color", lightColors[i]);

        // auto model = glm::mat4(1.0f);
        // model = glm::translate(model, newPos);
        // model = glm::scale(model, glm::vec3(0.7f));
        // pbrShader->SetUniformMatrix4fv("model", model);
        // pbrShader->SetUniformMatrix3fv("normalMatrix", glm::transpose(glm::inverse(glm::mat3(model))));

        // RenderSphere(camera, newPos, 1.0, {1.0f, 0.0f, 0.0f});
    }
}

void RenderSystem::SetupDefaultState()
{
    glFrontFace(GL_CCW);
    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);
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

    // set texture units
    // TODO: change uniform names
    auto modelShader = m_Shaders.at("modelShader");
    modelShader->SetUniform1i("diffuse0", 0);
    modelShader->SetUniform1i("normal", 1);
    modelShader->SetUniform1i("metallic", 2);
    modelShader->SetUniform1i("specular0", 3);
}

void RenderSystem::SetupScreenQuad()
{
    float quadVerts[] = {
        0.5f, 0.5f, 0.0f, 0.5f, -0.5f, 0.0f, -0.5f, -0.5f, 0.0f, -0.5f, 0.5f, 0.0f,
    };
    uint32_t quadIndices[] = {
        0, 1, 3, // first Triangle
        1, 2, 3  // second Triangle
    };

    m_QuadVAO.Init();
    m_QuadVAO.Bind();
    m_QuadVAO.AttachBuffer(BufferType::ARRAY, sizeof(quadVerts), DrawMode::DYNAMIC, nullptr);
    m_QuadVAO.AttachBuffer(BufferType::ELEMENT, sizeof(quadIndices), DrawMode::DYNAMIC, nullptr);
    m_QuadVAO.EnableAttribute(0, 3, 3 * sizeof(float), (void *)0);

    m_QuadVAO.SetBufferSubData(BufferType::ARRAY, 0, sizeof(quadVerts), quadVerts);
    m_QuadVAO.SetBufferSubData(BufferType::ELEMENT, 0, sizeof(quadIndices), quadIndices);
    m_QuadVAO.Unbind();
}

void RenderSystem::SetupLine()
{
    m_LineVAO.Init();
    m_LineVAO.Bind();

    m_LineVAO.AttachBuffer(BufferType::ARRAY, 6 * sizeof(float), DrawMode::DYNAMIC, nullptr);
    m_LineVAO.EnableAttribute(0, 3, 3 * sizeof(float), (void *)0);
    m_LineVAO.Unbind();
}

void RenderSystem::SetupSphere()
{
    m_SphereVAO.Init();
    m_SphereVAO.Bind();

    std::vector<glm::vec3> positions;
    std::vector<glm::vec2> uv;
    std::vector<glm::vec3> normals;
    std::vector<unsigned int> indices;

    const unsigned int X_SEGMENTS = 64;
    const unsigned int Y_SEGMENTS = 64;
    const float PI = 3.14159265359f;
    for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
    {
        for (unsigned int y = 0; y <= Y_SEGMENTS; ++y)
        {
            float xSegment = (float)x / (float)X_SEGMENTS;
            float ySegment = (float)y / (float)Y_SEGMENTS;
            float xPos = std::cos(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
            float yPos = std::cos(ySegment * PI);
            float zPos = std::sin(xSegment * 2.0f * PI) * std::sin(ySegment * PI);

            positions.push_back(glm::vec3(xPos, yPos, zPos));
            uv.push_back(glm::vec2(xSegment, ySegment));
            normals.push_back(glm::vec3(xPos, yPos, zPos));
        }
    }

    bool oddRow = false;
    for (unsigned int y = 0; y < Y_SEGMENTS; ++y)
    {
        if (!oddRow) // even rows: y == 0, y == 2; and so on
        {
            for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
            {
                indices.push_back(y * (X_SEGMENTS + 1) + x);
                indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
            }
        }
        else
        {
            for (int x = X_SEGMENTS; x >= 0; --x)
            {
                indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
                indices.push_back(y * (X_SEGMENTS + 1) + x);
            }
        }
        oddRow = !oddRow;
    }
    indexCount = static_cast<unsigned int>(indices.size());

    std::vector<float> data;
    for (unsigned int i = 0; i < positions.size(); ++i)
    {
        data.push_back(positions[i].x);
        data.push_back(positions[i].y);
        data.push_back(positions[i].z);
        if (normals.size() > 0)
        {
            data.push_back(normals[i].x);
            data.push_back(normals[i].y);
            data.push_back(normals[i].z);
        }
        if (uv.size() > 0)
        {
            data.push_back(uv[i].x);
            data.push_back(uv[i].y);
        }
    }

    m_SphereVAO.AttachBuffer(BufferType::ARRAY, data.size() * sizeof(float), DrawMode::STATIC, data.data());
    m_SphereVAO.AttachBuffer(BufferType::ELEMENT, indices.size() * sizeof(unsigned int), DrawMode::STATIC,
                             indices.data());

    unsigned int stride = (3 + 2 + 3) * sizeof(float);
    m_SphereVAO.EnableAttribute(0, 3, stride, (void *)0);
    m_SphereVAO.EnableAttribute(1, 3, stride, (void *)(3 * sizeof(float)));
    m_SphereVAO.EnableAttribute(2, 2, stride, (void *)(6 * sizeof(float)));

    m_SphereVAO.Unbind();
}

void RenderSystem::RenderModelsWithTextures(Camera &camera, Scene &scene)
{
    glBindSampler(m_SamplerPBRTextures, 0);
    glBindSampler(m_SamplerPBRTextures, 1);
    glBindSampler(m_SamplerPBRTextures, 2);
    glBindSampler(m_SamplerPBRTextures, 3);

    auto modelShader = m_Shaders.at("modelShader");

    auto view = scene.GetRegistry().view<ModelComponent, TransformComponent, VisibilityComponent>();
    for (auto entity : view)
    {
        auto [model, transform, visibility] = view.get<ModelComponent, TransformComponent, VisibilityComponent>(entity);
        if (!visibility.IsVisible || model.Model == nullptr) continue;

        auto boundingBox = model.Model->GetBoundingBoxes();

        for (auto mesh : model.Model->GetMeshes())
        {
            modelShader->Use();
            mesh.VAO.Bind();

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, mesh.Material->GetParameterTexture(Material::ALBEDO));
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, mesh.Material->GetParameterTexture(Material::NORMAL));
            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, mesh.Material->GetParameterTexture(Material::METALLIC));
            glActiveTexture(GL_TEXTURE3);
            glBindTexture(GL_TEXTURE_2D, mesh.Material->GetParameterTexture(Material::ROUGHNESS));

            modelShader->SetUniformMatrix4fv("model", transform.GetTransform());
            modelShader->SetUniformMatrix4fv("projectionViewMatrix", camera.GetProjectionViewMatrix());
            modelShader->SetUniform3f("gCameraPos", camera.GetPosition());

            // set materials
            // TODO: rewrite shader to fix this weirdness
            modelShader->SetUniform3f("gMaterial.AmbientColor", glm::vec3{1.0f});
            modelShader->SetUniform3f("gMaterial.DiffuseColor", glm::vec3{1.0f});
            modelShader->SetUniform3f("gMaterial.SpecularColor", glm::vec3{1.0f});

            Light::SetLightUniforms(*modelShader);

            glDrawElements(GL_TRIANGLES, mesh.IndexCount, GL_UNSIGNED_INT, nullptr);
            glBindTexture(GL_TEXTURE_2D, 0);

            // RenderBoundingBox(camera, boundingBox[0].Min, boundingBox[0].Max, glm::vec3{1.0f, 0.0f, 0.0f});

            // draws outline on selected model
            if (scene.GetSelectedEntity() == entity)
            {
            }

            mesh.VAO.Unbind();
        }
    }

    glBindSampler(m_SamplerPBRTextures, 0);
}

void RenderSystem::RenderModelsWithNoTextures(Camera &camera, Scene &scene) const
{
    auto modelShader = m_Shaders.at("modelShader");
    auto view = scene.GetRegistry().view<ModelComponent, TransformComponent>();
    for (auto entity : view)
    {
        auto [model, transform] = view.get<ModelComponent, TransformComponent>(entity);
        if (model.Model == nullptr) continue;

        for (auto mesh : model.Model->GetMeshes())
        {
            modelShader->Use();
            mesh.VAO.Bind();

            modelShader->SetUniformMatrix4fv("model", transform.GetTransform());
            modelShader->SetUniformMatrix4fv("projectionViewMatrix", camera.GetProjectionViewMatrix());

            glDrawElements(GL_TRIANGLES, mesh.IndexCount, GL_UNSIGNED_INT, nullptr);
            mesh.VAO.Unbind();
        }
    }
}

void RenderSystem::RenderQuad(Camera &camera)
{
    auto modelShader = m_Shaders.at("quadShader");
    modelShader->Use();
    modelShader->SetUniformMatrix4fv("projectionViewMatrix", camera.GetProjectionViewMatrix());

    m_QuadVAO.Bind();

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    m_QuadVAO.Unbind();
}

void RenderSystem::RenderLine(Camera &camera, const glm::vec3 &start, const glm::vec3 &end, const glm::vec3 &color)
{
    float vertices[] = {start.x, start.y, start.z, end.x, end.y, end.z};
    m_LineVAO.SetBufferSubData(BufferType::ARRAY, 0, sizeof(vertices), vertices);

    auto lineShader = m_Shaders.at("lineShader");
    lineShader->Use();
    m_LineVAO.Bind();

    lineShader->SetUniformMatrix4fv("projectionViewMatrix", camera.GetProjectionViewMatrix());
    lineShader->SetUniform3f("color", color);

    glDrawArrays(GL_LINES, 0, 2);
    m_LineVAO.Unbind();
}

void RenderSystem::RenderSphere(Camera &camera, const glm::vec3 &position, const float radius, const glm::vec3 &color)
{
    m_SphereVAO.Bind();
    auto pbrShader = m_Shaders.at("pbrShader");
    pbrShader->Use();

    pbrShader->SetUniformMatrix4fv("projectionViewMatrix", camera.GetProjectionViewMatrix());

    pbrShader->SetUniform3f("albedo", color);
    pbrShader->SetUniform1f("metallic", 0.05f);
    pbrShader->SetUniform1f("roughness", 0.15f);
    pbrShader->SetUniform1f("ao", 1.0f);

    pbrShader->SetUniform3f("cameraPosition", camera.GetPosition());
    pbrShader->SetUniform1i("numOfPointLights", 4);

    // pbrShader->SetUniform3f("gPointLights[0].Position", glm::vec3{0.0f, 0.0f, -1.0f});
    // pbrShader->SetUniform3f("gPointLights[0].Color", glm::vec3{1.0f});

    glDrawElements(GL_TRIANGLE_STRIP, indexCount, GL_UNSIGNED_INT, nullptr);
    m_SphereVAO.Unbind();
}
} // namespace Engine