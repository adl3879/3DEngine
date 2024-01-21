#pragma once

#include <glm/glm.hpp>

#include "ShaderManager.h"
#include "StaticMesh.h"
#include "Camera.h"
#include "RenderList.h"
#include "VertexArray.h"
#include "Material.h"
#include "Texture2D.h"
#include "SkinnedMesh.h"

namespace Engine
{
class Renderer
{
public:
    static void Init();

    static void SubmitMesh(StaticMeshRef mesh, MaterialRef mat, glm::mat4 &transform, const int32_t entityId = -1);
	static void SubmitSkinnedMesh(SkinnedMeshRef mesh, MaterialRef mat, glm::mat4 &transform, const int32_t entityId = -1);
    static void Flush(Shader *shader, bool depthOnly = false);

    // drawing states
    static void BeginDraw(CameraRef camera);
    static void EndDraw();

    static void DrawQuad(glm::mat4 transform = glm::mat4());
    static void DrawSprite(const glm::mat4 &projection, const glm::mat4 &view, glm::mat4 model, Texture2DRef texture);
    static void DrawCameraFrustum(const glm::mat4 &projection, const glm::mat4 &view, glm::mat4 model = glm::mat4());

    // debug
    static void DrawCube();
    static void DrawBoxCollider(const glm::mat4 &projection, const glm::mat4 &view, const glm::mat4 &transform = glm::mat4());
    static void DrawSphereCollider(const glm::mat4 &projection, const glm::mat4 &view, const glm::mat4 &transform = glm::mat4());

public:
    static Shader *m_Shader;
    static VertexArray *QuadVAO;
    static VertexArray *FrustumVAO;
    static VertexArray *CubeVAO;

    static StaticMeshRef SphereMesh;

private:
    static RenderList m_RenderList;

private:
    static StaticMeshRef CreateSphereMesh();
};
} // namespace Engine
