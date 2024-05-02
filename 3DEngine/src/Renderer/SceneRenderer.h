#pragma once

#include <glm/glm.hpp>

#include "Renderer.h"
#include "Scene.h"
#include "Framebuffer.h"

#include <memory>

namespace Engine
{
class SceneRenderer
{
public:
    void Init();
    void Cleanup();

    void BeginRenderScene();
    void RenderScene(Scene &scene, Framebuffer &framebuffer);

    bool SetShowDebug(bool show) { return m_ShowDebug = show; }

private:
    void ShadowPass(Scene &scene);
    void EnvironmentPass(Scene &scene);
    void LightingPass(Scene &scene);
	void OutlinePass(Scene &scene);

private:
    FramebufferRef m_HDRBuffer;
    FramebufferRef m_ShadingBuffer;
    FramebufferRef m_OutlineBuffer;
    FramebufferRef m_Edge;

    FramebufferRef m_ShadowBuffer;

private:
    bool m_ShowDebug = true;
};

using SceneRendererRef = std::shared_ptr<SceneRenderer>;
} // namespace Engine
