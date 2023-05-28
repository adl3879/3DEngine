#include "Application.h"
#include "Buffer.h"
#include "Input/InputDevice.h"
#include "Input/InputKey.h"
#include "Input/InputManager.h"

#include <functional>
#include <iostream>
#include <memory>

float lastFrame = 0.0f;

Application::Application() : m_IsRunning(true)
{

    m_Window = std::make_shared<Window>(WindowProps());

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
    }

    SetupInputSystem();

    // Setup OpenGL options
    glEnable(GL_DEPTH_TEST);

    m_ModelShader = std::make_unique<Shader>("../res/shaders/model.vert", "../res/shaders/model.frag");

    m_ModelShader->Use();
    glm::mat4 model3{1.0f};
    m_ModelShader->SetUniformMatrix4fv("model", model3);
    m_ModelShader->SetUniform4f("lightColor", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
    m_ModelShader->SetUniform3f("lightPos", glm::vec3(0.3f, 0.6f, 0.0f));
    m_ModelShader->SetUniform3f("cameraPos", m_Camera.GetPosition());

    // m_Model = std::make_unique<Model>("../res/models/boxTextured/scene.gltf");
    m_Model = std::make_unique<Model>("../res/models/suzanne/scene.gltf");
}

Application &Application::Instance()
{
    static Application app;
    return app;
}

void Application::Run()
{
    while (m_IsRunning && !glfwWindowShouldClose(m_Window->GetNativeWindow()))
    {
        // Define the viewport dimensions
        auto windowState = InputManager::Instance().GetWindowState();
        glViewport(0, 0, windowState.Width, windowState.Height);

        glfwPollEvents();
        // doMovement();
        InputManager::Instance().ProcessInput();

        // delta time
        float currentFrame = glfwGetTime();
        m_DeltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        m_CameraController.OnUpdate(m_DeltaTime);

        glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        m_Model->Draw(*m_ModelShader, m_Camera);

        // Swap the screen buffers
        glfwSwapBuffers(m_Window->GetNativeWindow());
    }
}

void Application::SetupInputSystem()
{
    InputManager::Instance().MapInputToAction(InputKey::Escape, InputAction{.ActionName = "quit", .Scale = 1.0f});
    InputManager::Instance().RegisterActionCallback(
        "quit", InputManager::ActionCallback{.Ref = "quit",
                                             .Func = [this](InputSource source, int value, float scale)
                                             {
                                                 m_IsRunning = false;
                                                 return true;
                                             }});
}

Application::~Application() { glfwTerminate(); }
