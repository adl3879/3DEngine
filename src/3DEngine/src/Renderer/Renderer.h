#pragma once

#include <glm/glm.hpp>

#include "ShaderManager.h"
#include "Mesh.h"
#include "Camera.h"
#include "RenderList.h"

namespace Engine
{
class Renderer
{
  public:
    static void Init();

    static void SubmitMesh(MeshRef mesh, const glm::mat4 &transform, const int32_t entityId = -1);
    static void Flush(Shader *shader, bool depthOnly = false);

    // drawing states
    static void BeginDraw(CameraRef camera);
    static void EndDraw();

    // lights
    // debug
  public:
    static Shader *m_Shader;

  private:
    static RenderList m_RenderList;
};
} // namespace Engine