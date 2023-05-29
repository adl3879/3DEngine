#pragma once

#include "Shader.h"
#include "Layer.h"
#include "CameraController.h"
#include "Model.h"
#include "Camera.h"

#include <memory>

class AppLayer : public Layer
{
  public:
    AppLayer();
    virtual ~AppLayer() = default;

    virtual void OnAttach() override;
    virtual void OnDetach() override;
    virtual void OnUpdate(float deltaTime) override;

  private:
    std::unique_ptr<Shader> m_ModelShader;
    std::unique_ptr<Model> m_Model;

    Camera m_Camera{};
    CameraController m_CameraController{m_Camera, 0.1f, 2.5f};
};