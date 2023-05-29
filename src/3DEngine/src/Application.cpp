#include "Application.h"
#include "InputDevice.h"
#include "InputKey.h"
#include "InputManager.h"

#include <functional>
#include <iostream>
#include <memory>
#include <algorithm>

float lastFrame = 0.0f;

Application::Application() : m_IsRunning(true)
{

    m_Window = std::make_shared<Window>(WindowProps());

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
    }
    // Setup OpenGL options
    glEnable(GL_DEPTH_TEST);

    SetupInputSystem();

    m_ImGuiLayer = new ImGuiLayer();
    PushOverlay(m_ImGuiLayer);
}

void Application::Run()
{
    while (m_IsRunning && !glfwWindowShouldClose(m_Window->GetNativeWindow()))
    {
        // doMovement();
        m_Window->OnUpdate();
        InputManager::Instance().ProcessInput();

        // delta time
        float currentFrame = glfwGetTime();
        m_DeltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        for (Layer *layer : m_LayerStack)
            layer->OnUpdate(m_DeltaTime);

        m_ImGuiLayer->Begin();
        {
            for (Layer *layer : m_LayerStack)
                layer->OnImGuiRender();
        }
        m_ImGuiLayer->End();

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

void Application::PushLayer(Layer *layer)
{
    m_LayerStack.PushLayer(layer);
    layer->OnAttach();

    InputManager::Instance().RegisterCursorCallback(
        std::bind(&Layer::OnMouseMoved, layer, std::placeholders::_1, std::placeholders::_2));
    InputManager::Instance().RegisterKeyboardCallback(
        std::bind(&Layer::OnKeyPressed, layer, std::placeholders::_1, std::placeholders::_2));
}

void Application::PushOverlay(Layer *layer)
{
    m_LayerStack.PushOverlay(layer);
    layer->OnAttach();
    InputManager::Instance().RegisterCursorCallback(
        std::bind(&Layer::OnMouseMoved, layer, std::placeholders::_1, std::placeholders::_2));
    InputManager::Instance().RegisterKeyboardCallback(
        std::bind(&Layer::OnKeyPressed, layer, std::placeholders::_1, std::placeholders::_2));
}