#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <memory>

#include "Buffer.h"
#include "Input/Input.h"
#include "Shader.h"
#include "Texture.h"
#include "VertexArray.h"

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

  private:
    Application();
    void SetupInputSystem();

  private:
    GLFWwindow *m_Window;
    bool m_IsRunning;
    float m_DeltaTime = 0.0f;
    Input m_Input{};

    std::unique_ptr<Shader> m_Shader;
    std::unique_ptr<Texture> m_Texture;

    std::unique_ptr<VertexArray> m_VAO;
    std::unique_ptr<VertexBuffer> m_VBO;
    std::unique_ptr<IndexBuffer> m_EBO;
};
