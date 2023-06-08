#include "Application.h"

#include "InputManager.h"

#include <functional>
#include <iostream>
#include <memory>
#include <algorithm>

namespace Engine
{
float lastFrame = 0.0f;

std::shared_ptr<Window> Application::m_Window = nullptr;

Application::Application() : m_IsRunning(true)
{
    m_Window = std::make_shared<Window>(WindowProps());

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

void Application::RegisterLayerEventCallbacks(Layer *layer)
{
    InputManager::Instance().RegisterKeyboardCallback(
        std::bind(&Layer::OnKeyPressed, layer, std::placeholders::_1, std::placeholders::_2));
    InputManager::Instance().RegisterKeyReleasedCallback(
        std::bind(&Layer::OnKeyReleased, layer, std::placeholders::_1));
    InputManager::Instance().RegisterMousePressedCallback(
        std::bind(&Layer::OnMouseButtonPressed, layer, std::placeholders::_1));
    InputManager::Instance().RegisterMouseReleasedCallback(
        std::bind(&Layer::OnMouseButtonReleased, layer, std::placeholders::_1));
    InputManager::Instance().RegisterWindowResizeCallback(
        std::bind(&Layer::OnWindowResize, layer, std::placeholders::_1, std::placeholders::_2));
    InputManager::Instance().RegisterMouseScrollCallback(
        std::bind(&Layer::OnMouseScrolled, layer, std::placeholders::_1, std::placeholders::_2));
    InputManager::Instance().RegisterMouseMovedCallback(std::bind(&Layer::OnMouseMoved, layer, std::placeholders::_1,
                                                                  std::placeholders::_2, std::placeholders::_3,
                                                                  std::placeholders::_4));
}

Application::~Application() { glfwTerminate(); }

void Application::PushLayer(Layer *layer)
{
    m_LayerStack.PushLayer(layer);
    layer->OnAttach();
    RegisterLayerEventCallbacks(layer);
}

void Application::PushOverlay(Layer *layer)
{
    m_LayerStack.PushOverlay(layer);
    layer->OnAttach();
    RegisterLayerEventCallbacks(layer);
}
} // namespace Engine