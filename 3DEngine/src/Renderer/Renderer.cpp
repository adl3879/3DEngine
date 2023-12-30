#include "Renderer.h"

#include "RenderCommand.h"
#include "ShaderManager.h"

#include <glad/glad.h>

namespace Engine
{
Shader *Renderer::m_Shader = nullptr;
VertexArray *Renderer::QuadVAO = nullptr;
VertexArray *Renderer::FrustumVAO = nullptr;
VertexArray *Renderer::CubeVAO = nullptr;
RenderList Renderer::m_RenderList;

MeshRef Renderer::SphereMesh = nullptr;

struct RVertex
{
    glm::vec3 Position;
    glm::vec2 UV;
    glm::vec3 Normal;
};

// clang-format off
std::vector<RVertex> QuadVertices
{
    {glm::vec3(-1.0f, 1.0f, 0.0f), glm::vec2(0.0f, 1.0f), glm::vec3(0, 0, -1)},
    {glm::vec3(1.0f, 1.0f, 0.0f), glm::vec2(1.0f, 1.0f), glm::vec3(0, 0, -1)},
    {glm::vec3(-1.0f, -1.0f, 0.0f), glm::vec2(0.0f, 0.0f), glm::vec3(0, 0, -1)},
    {glm::vec3(1.0f, -1.0f, 0.0f), glm::vec2(1.0f, 0.0f), glm::vec3(0, 0, -1)},
    {glm::vec3(-1.0f, -1.0f, 0.0f), glm::vec2(0.0f, 0.0f), glm::vec3(0, 0, -1)},
    {glm::vec3(1.0f, 1.0f, 0.0f), glm::vec2(1.0f, 1.0f), glm::vec3(0, 0, -1)}
};

std::vector<glm::vec3> FrustumVertices
{
    // Near plane
    glm::vec3(-1.0f, -1.0f, -1.0f),
    glm::vec3(1.0f, -1.0f, -1.0f),
    glm::vec3(1.0f, 1.0f, -1.0f),
    glm::vec3(-1.0f, 1.0f, -1.0f),
    // Far plane
    glm::vec3(0.0f, 0.0f, 0.0f),
};

std::vector<uint32_t> FrustumIndices
{
    0, 1, 2, 3, 0,
    0, 1, 4, 2, 3, 4
};

float CubeVertices[] = {
    // back face
    -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
    1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,   // top-right
    1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f,  // bottom-right
    1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,   // top-right
    -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
    -1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f,  // top-left
    // front face
    -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom-left
    1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,  // bottom-right
    1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,   // top-right
    1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,   // top-right
    -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,  // top-left
    -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom-left
    // left face
    -1.0f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,   // top-right
    -1.0f, 1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f,  // top-left
    -1.0f, -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-left
    -1.0f, -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-left
    -1.0f, -1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,  // bottom-right
    -1.0f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,   // top-right
    // right face
    1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,   // top-left
    1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-right
    1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,  // top-right
    1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-right
    1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,   // top-left
    1.0f, -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,  // bottom-left
    // bottom face
    -1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, // top-right
    1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f,  // top-left
    1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,   // bottom-left
    1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,   // bottom-left
    -1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f,  // bottom-right
    -1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, // top-right
    // top face
    -1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, // top-left
    1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,   // bottom-right
    1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,  // top-right
    1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,   // bottom-right
    -1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, // top-left
    -1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f   // bottom-left
};
// clang-format on

void Renderer::Init()
{
    QuadVAO = new VertexArray();
    QuadVAO->Init();
    QuadVAO->Bind();
    QuadVAO->AttachBuffer(BufferType::ARRAY, QuadVertices.size() * sizeof(RVertex), DrawMode::STATIC,
                          QuadVertices.data());

    // vertex attributes
    const static auto vertexSize = sizeof(RVertex);
    QuadVAO->EnableAttribute(0, 3, vertexSize, reinterpret_cast<void *>(0));
    QuadVAO->EnableAttribute(1, 3, vertexSize, reinterpret_cast<void *>(offsetof(RVertex, UV)));
    QuadVAO->EnableAttribute(2, 3, vertexSize, reinterpret_cast<void *>(offsetof(RVertex, Normal)));
    QuadVAO->Unbind();

    SphereMesh = CreateSphereMesh();

    // camera frustum
    FrustumVAO = new VertexArray();
    FrustumVAO->Init();
    FrustumVAO->Bind();
    FrustumVAO->AttachBuffer(BufferType::ARRAY, FrustumVertices.size() * sizeof(glm::vec3), DrawMode::STATIC,
                             FrustumVertices.data());
    FrustumVAO->AttachBuffer(BufferType::ELEMENT, FrustumIndices.size() * sizeof(uint32_t), DrawMode::STATIC,
                             FrustumIndices.data());
    FrustumVAO->EnableAttribute(0, 3, sizeof(glm::vec3), reinterpret_cast<void *>(0));
    FrustumVAO->Unbind();

    // cube
    CubeVAO = new VertexArray();
    CubeVAO->Init();
    CubeVAO->Bind();
    CubeVAO->AttachBuffer(BufferType::ARRAY, sizeof(CubeVertices), DrawMode::STATIC, CubeVertices);
    CubeVAO->EnableAttribute(0, 3, 8 * sizeof(float), nullptr);
    CubeVAO->EnableAttribute(1, 3, 8 * sizeof(float), reinterpret_cast<void *>(3 * sizeof(float)));
    CubeVAO->EnableAttribute(2, 2, 8 * sizeof(float), reinterpret_cast<void *>(6 * sizeof(float)));
    CubeVAO->Unbind();
}

void Renderer::SubmitMesh(MeshRef mesh, MaterialRef mat, glm::mat4 &transform, const int32_t entityId)
{
    m_RenderList.AddToRenderList(mesh, mat, transform, entityId);
}

void Renderer::Flush(Shader *shader, bool depthOnly) { m_RenderList.Flush(shader, depthOnly); }

void Renderer::BeginDraw(CameraRef camera)
{
}

void Renderer::EndDraw()
{
}

void Renderer::DrawQuad(glm::mat4 transform)
{
    QuadVAO->Bind();
    RenderCommand::DrawArrays(0, 6);
}

MeshRef Renderer::CreateSphereMesh()
{
    const float sectorCount = 36;
    const float stackCount = 36;
    const float radius = 0.5f;
    const float PI = acos(-1.0f);

    // new
    std::vector<Vertex> finalVertices;

    float x, y, z, xy;                           // vertex position
    float nx, ny, nz, lengthInv = 1.0f / radius; // normal
    float s, t;                                  // texCoord

    float sectorStep = 2 * PI / sectorCount;
    float stackStep = PI / stackCount;
    float sectorAngle, stackAngle;

    for (int i = 0; i <= stackCount; ++i)
    {
        stackAngle = PI / 2 - i * stackStep; // starting from pi/2 to -pi/2
        xy = radius * cosf(stackAngle);      // r * cos(u)
        z = radius * sinf(stackAngle);       // r * sin(u)

        // add (sectorCount+1) vertices per stack
        // the first and last vertices have same position and normal, but different tex coords
        for (int j = 0; j <= sectorCount; ++j)
        {
            sectorAngle = j * sectorStep; // starting from 0 to 2pi

            Vertex newVertex;

            x = xy * cosf(sectorAngle); // r * cos(u) * cos(v)
            y = xy * sinf(sectorAngle); // r * cos(u) * sin(v)
            newVertex.Position = glm::vec3(x, y, z);

            nx = x * lengthInv;
            ny = y * lengthInv;
            nz = z * lengthInv;
            newVertex.Normal = glm::vec3(nx, ny, nz) * -1.0f;
            // vertex position

            s = (float)j / sectorCount * 4.f;
            t = (float)i / stackCount * 4.f;
            newVertex.TexCoords = {t, s};

            finalVertices.push_back(newVertex);
        }
    }

    std::vector<uint32_t> finalIndices;
    unsigned int k1, k2;
    for (int i = 0; i < stackCount; ++i)
    {
        k1 = i * (sectorCount + 1); // beginning of current stack
        k2 = k1 + sectorCount + 1;  // beginning of next stack

        for (int j = 0; j < sectorCount; ++j, ++k1, ++k2)
        {
            // 2 triangles per sector excluding 1st and last stacks
            if (i != 0)
            {
                finalIndices.push_back(k1);
                finalIndices.push_back(k2);
                finalIndices.push_back(k1 + 1);
            }

            if (i != (stackCount - 1))
            {
                finalIndices.push_back(k1 + 1);
                finalIndices.push_back(k2);
                finalIndices.push_back(k2 + 1);
            }
        }
    }

    MeshRef mesh = std::make_shared<Mesh>("sphere", finalVertices, finalIndices);
    return mesh;
}

void Renderer::DrawCameraFrustum(const glm::mat4 &projection, const glm::mat4 &view, glm::mat4 model)
{
    FrustumVAO->Bind();

    const auto shader = ShaderManager::GetShader("Resources/shaders/frustum");
    shader->Bind();

    // scale the frustum
    model = glm::scale(model, glm::vec3(0.8f, 0.4f, 1.3f));

    shader->SetUniformMatrix4fv("model", model);
    shader->SetUniformMatrix4fv("view", view);
    shader->SetUniformMatrix4fv("projection", projection);

    RenderCommand::DrawLineStrip(0, 14);
    FrustumVAO->Unbind();
}

void Renderer::DrawCube()
{
    CubeVAO->Bind();
    RenderCommand::DrawArrays(0, 36);
    CubeVAO->Unbind();
}

void Renderer::DrawBoxCollider(const glm::mat4 &projection, const glm::mat4 &view, const glm::mat4 &transform)
{
    CubeVAO->Bind();

    const auto shader = ShaderManager::GetShader("Resources/shaders/debug");
    shader->Bind();
    shader->SetUniform3f("color", {0.0f, 1.0f, 0.0f});
    shader->SetUniformMatrix4fv("model", transform);
    shader->SetUniformMatrix4fv("view", view);
    shader->SetUniformMatrix4fv("projection", projection);

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    RenderCommand::DrawArrays(0, 36);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    CubeVAO->Unbind();
}

void Renderer::DrawSphereCollider(const glm::mat4 &projection, const glm::mat4 &view, const glm::mat4 &transform)
{
    SphereMesh->VAO.Bind();

    const auto shader = ShaderManager::GetShader("Resources/shaders/debug");
    shader->Bind();
    shader->SetUniform3f("color", {0.0f, 1.0f, 0.0f});
    shader->SetUniformMatrix4fv("model", transform);
    shader->SetUniformMatrix4fv("view", view);
    shader->SetUniformMatrix4fv("projection", projection);

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    RenderCommand::DrawElements(RendererEnum::TRIANGLES, SphereMesh->IndexCount, RendererEnum::UINT, nullptr);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    SphereMesh->VAO.Unbind();
}

void Renderer::DrawSprite(const glm::mat4 &projection, const glm::mat4 &view, glm::mat4 model, Texture2DRef texture)
{
    QuadVAO->Bind();

    const auto shader = ShaderManager::GetShader("Resources/shaders/sprite");
    shader->Bind();
    shader->SetUniformMatrix4fv("model", model);
    shader->SetUniformMatrix4fv("view", view);
    shader->SetUniformMatrix4fv("projection", projection);

    shader->SetUniform1i("u_Texture", 0);
    texture->Bind(0);

    RenderCommand::DrawArrays(0, 6);
    QuadVAO->Unbind();
}
} // namespace Engine
