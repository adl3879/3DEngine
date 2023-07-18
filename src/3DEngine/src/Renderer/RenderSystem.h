#pragma once

#include "Model.h"
#include "Camera.h"
#include "Scene.h"
#include "Shader.h"
#include "SkyLight.h"

#include <unordered_map>

namespace Engine
{
class RenderSystem
{
  public:
    RenderSystem() = default;
    virtual ~RenderSystem() = default;

  public:
    void Init();
    void Update(const Camera &camera);
    void Shutdown();

    void Render(Camera &camera, Scene &scene, const bool globalWireframe = false);

  private:
    void SetupDefaultState();
    void SetupTextureSamplers();
    void SetupScreenQuad();
    void SetupLine();
    void SetupSphere();

  private:
    void RenderModelsWithTextures(Camera &camera, Scene &scene);
    void RenderModelsWithNoTextures(Camera &camera, Scene &scene) const;
    void RenderQuad(Camera &camera);
    void RenderLine(Camera &camera, const glm::vec3 &start, const glm::vec3 &end, const glm::vec3 &color);
    void RenderSphere(Camera &camera, const glm::vec3 &position, const float radius, const glm::vec3 &color);

  private:
    unsigned int m_UBOMatrices = 0;
    unsigned int m_SamplerPBRTextures = 0;

    SkyLightPtr m_SkyLight = nullptr;

    VertexArray m_QuadVAO;
    VertexArray m_LineVAO;
    VertexArray m_BoundingBoxVAO;
    VertexArray m_SphereVAO;

  private:
    std::unordered_map<std::string, ShaderPtr> m_Shaders;
};

using RenderSystemPtr = std::shared_ptr<RenderSystem>;
} // namespace Engine