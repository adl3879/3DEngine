#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <memory>

#include "CameraController.h"
#include "Mesh.h"
#include "Buffer.h"
#include "Input/Input.h"
#include "Shader.h"
#include "Texture.h"
#include "VertexArray.h"
#include "Model.h"

constexpr int windowWidth = 1000;
constexpr int windowHeight = 800;

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

    GLFWwindow *GetWindow() const { return m_Window; }

  private:
    Application();
    void SetupInputSystem();

  private:
    GLFWwindow *m_Window;
    bool m_IsRunning;
    float m_DeltaTime = 0.0f;
    Input m_Input{};

    std::unique_ptr<Shader> m_Shader;
    std::unique_ptr<Shader> m_LightShader;
    std::unique_ptr<Shader> m_ModelShader;

    std::unique_ptr<Mesh> m_Mesh;
    std::unique_ptr<Mesh> m_LightMesh;

    std::unique_ptr<Model> m_Model;

    Camera m_Camera{};
    CameraController m_CameraController{m_Camera, 0.1f, 2.5f};
};
