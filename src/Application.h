#pragma once

#include <glad/glad.h>
#include <memory>

#include "Window.h"
#include "CameraController.h"
#include "Buffer.h"
#include "Input/Input.h"
#include "Shader.h"
#include "Texture.h"
#include "VertexArray.h"
#include "Model.h"

class Application
{
  public:
    Application(Application const &app) = delete;
    Application &operator=(Application const &app) = delete;
    virtual ~Application();

    static Application &Instance();
    void Run();
    void Close() { m_IsRunning = false; }

    bool IsRunning() const { return m_IsRunning; }
    float GetDeltaTime() const { return m_DeltaTime; }

    // GLFWwindow *GetNativeWindow() const { return m_Window->GetNativeWindow(); }
    const std::shared_ptr<Window> &GetWindow() const { return m_Window; }

  private:
    Application();
    void SetupInputSystem();

  private:
    std::shared_ptr<Window> m_Window;

    bool m_IsRunning;
    float m_DeltaTime = 0.0f;

    std::unique_ptr<Shader> m_ModelShader;
    std::unique_ptr<Model> m_Model;

    Camera m_Camera{};
    CameraController m_CameraController{m_Camera, 0.1f, 2.5f};
};
