#include "Renderer.h"

#include "RenderCommand.h"

namespace Engine
{
Shader *Renderer::m_Shader = nullptr;
VertexArray *Renderer::QuadVAO = nullptr;
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
    { glm::vec3(-1.0f,  1.0f, 0.0f), glm::vec2(0.0f, 1.0f), glm::vec3(0, 0, -1) },
    { glm::vec3(1.0f,  1.0f, 0.0f),  glm::vec2(1.0f, 1.0f), glm::vec3(0, 0, -1) },
    { glm::vec3(-1.0f, -1.0f, 0.0f), glm::vec2(0, 0),       glm::vec3(0, 0, -1) },
    { glm::vec3(1.0f,  -1.0f, 0.0f), glm::vec2(1.0f, 0.0f), glm::vec3(0, 0, -1) },
    { glm::vec3(-1.0f, -1.0f, 0.0f), glm::vec2(0.0f, 0.0f), glm::vec3(0, 0, -1) },
    { glm::vec3(1.0f,   1.0f, 0.0f), glm::vec2(1.0f, 1.0f), glm::vec3(0, 0, -1) }
};
// clang-format on

void Renderer::Init()
{
    //
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
}

void Renderer::SubmitMesh(MeshRef mesh, glm::mat4 &transform, const int32_t entityId)
{
    m_RenderList.AddToRenderList(mesh, transform, entityId);
}

void Renderer::Flush(Shader *shader, bool depthOnly) { m_RenderList.Flush(shader, depthOnly); }

void Renderer::BeginDraw(CameraRef camera) {}

void Renderer::EndDraw() {}

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

	MeshRef mesh = std::make_shared<Mesh>(finalVertices, finalIndices);
	return mesh;
}
} // namespace Engine
