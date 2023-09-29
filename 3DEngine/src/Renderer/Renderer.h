#pragma once

#include <glm/glm.hpp>

#include "ShaderManager.h"
#include "Mesh.h"
#include "Camera.h"
#include "RenderList.h"
#include "VertexArray.h"

namespace Engine
{
class Renderer
{
  public:
    static void Init();

    static void SubmitMesh(MeshRef mesh, glm::mat4 &transform, const int32_t entityId = -1);
    static void Flush(Shader *shader, bool depthOnly = false);

    // drawing states
    static void BeginDraw(CameraRef camera);
    static void EndDraw();

    // lights

    static void DrawQuad(glm::mat4 transform = glm::mat4());

  public:
    static Shader *m_Shader;

  public:
    static VertexArray *QuadVAO;

  private:
    static RenderList m_RenderList;
};
} // namespace Engine
