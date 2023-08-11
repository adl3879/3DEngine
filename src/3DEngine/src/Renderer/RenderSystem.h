#pragma once

#include "Model.h"
#include "Camera.h"
#include "Scene.h"
#include "Shader.h"
#include "SkyLight.h"
#include "Framebuffer.h"

#include <unordered_map>
#include <memory>

namespace Engine
{
class OutlineSystem;

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

  private:
    void RenderModelsWithTextures(Camera &camera, Scene &scene);
    void RenderModelsWithNoTextures(Camera &camera, Scene &scene) const;
    void RenderQuad(Camera &camera);

  private:
    unsigned int m_UBOMatrices = 0;
    unsigned int m_SamplerPBRTextures = 0;

    VertexArray m_QuadVAO;
    VertexArray m_LineVAO;
    VertexArray m_BoundingBoxVAO;
    VertexArray m_SphereVAO;

  private:
    std::unordered_map<std::string, ShaderPtr> m_Shaders;

  private:
    std::shared_ptr<OutlineSystem> m_Outline = nullptr;
    FramebufferPtr m_FBO = nullptr;
};

using RenderSystemPtr = std::shared_ptr<RenderSystem>;
} // namespace Engine