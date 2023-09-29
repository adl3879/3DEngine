#include "Application.h"

#include "InputManager.h"
#include "Log.h"

#include <GLFW/glfw3.h>

namespace Engine
{
float lastFrame = 0.0f;

std::shared_ptr<Window> Application::m_Window = nullptr;
bool Application::m_IsRunning = true;
bool Application::m_Minimized = false;
std::vector<std::function<void()>> Application::m_MainThreadQueue;
std::mutex Application::m_MainThreadQueueMutex;

Application::Application()
{
    Log::Init();
    m_Window = std::make_shared<Window>(WindowProps());

    SetupInputSystem();

    m_ImGuiLayer = new ImGuiLayer();
    PushOverlay(m_ImGuiLayer);

    LOG_CORE_TRACE("Engine Initialized");
}

void Application::SubmitToMainThread(const std::function<void()> &func) 
{
	std::scoped_lock<std::mutex> lock(m_MainThreadQueueMutex);
	m_MainThreadQueue.emplace_back(func);
}

void Application::Run()
{
    while (m_IsRunning && !glfwWindowShouldClose(m_Window->GetNativeWindow()))
    {
		ExecuteMainThreadQueue();
        if (!m_Minimized)
        {
            // doMovement();
            m_Window->OnUpdate();
            InputManager::Instance().ProcessInput();

            // delta time
            float currentFrame = glfwGetTime();
            m_DeltaTime = currentFrame - lastFrame;
            lastFrame = currentFrame;

            for (Layer *layer : m_LayerStack)
            {
                layer->OnUpdate(m_DeltaTime);

                float fixedTime = 1 / 90.0f; // TODO: make this configurable
                layer->OnFixedUpdate(fixedTime);
            }

            m_ImGuiLayer->Begin();
            for (Layer *layer : m_LayerStack) layer->OnImGuiRender();
            m_ImGuiLayer->End();

            // Swap the screen buffers
            glfwSwapBuffers(m_Window->GetNativeWindow());
        }
    }
}

void Application::SetupInputSystem() {}

void Application::RegisterLayerEventCallbacks(Layer *layer)
{
    static InputManager &Input = InputManager::Instance();

    Input.RegisterKeyboardCallback(
        std::bind(&Layer::OnKeyPressed, layer, std::placeholders::_1, std::placeholders::_2));
    Input.RegisterKeyReleasedCallback(std::bind(&Layer::OnKeyReleased, layer, std::placeholders::_1));
    Input.RegisterMousePressedCallback(std::bind(&Layer::OnMouseButtonPressed, layer, std::placeholders::_1));
    Input.RegisterMouseReleasedCallback(std::bind(&Layer::OnMouseButtonReleased, layer, std::placeholders::_1));
    Input.RegisterWindowResizeCallback(
        std::bind(&Layer::OnWindowResize, layer, std::placeholders::_1, std::placeholders::_2));
    Input.RegisterMouseScrollCallback(
        std::bind(&Layer::OnMouseScrolled, layer, std::placeholders::_1, std::placeholders::_2));
    Input.RegisterMouseMovedCallback(std::bind(&Layer::OnMouseMoved, layer, std::placeholders::_1,
                                               std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
}

void Application::ExecuteMainThreadQueue() 
{
    std::scoped_lock<std::mutex> lock(m_MainThreadQueueMutex);

	for (auto &func : m_MainThreadQueue)
		func();
	m_MainThreadQueue.clear();
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
