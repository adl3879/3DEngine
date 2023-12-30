#pragma once

#include "Window.h"
#include "LayerStack.h"
#include "ImGuiLayer.h"

#include <functional>
#include <memory>
#include <mutex>

int main(int argc, char **argv);

namespace Engine
{
class Application
{
  public:
    Application();

    virtual ~Application();

    void PushLayer(Layer *layer);
    void PushOverlay(Layer *layer);

    static void Close() { m_IsRunning = false; }

    bool IsRunning() const { return m_IsRunning; }
    float GetDeltaTime() const { return m_DeltaTime; }

    // GLFWwindow *GetNativeWindow() const { return m_Window->GetNativeWindow(); }
    static const std::shared_ptr<Window> &GetWindow() { return m_Window; }

    static void SubmitToMainThread(const std::function<void()> &func);

  private:
    void Run();
    void SetupInputSystem();
    void RegisterLayerEventCallbacks(Layer *layer);

    void ExecuteMainThreadQueue();

  private:
    static std::shared_ptr<Window> m_Window;

    static bool m_IsRunning;
    static bool m_Minimized;
    float m_DeltaTime = 0.0f;

    LayerStack m_LayerStack{};
    ImGuiLayer *m_ImGuiLayer;

    static std::vector<std::function<void()>> m_MainThreadQueue;
    static std::mutex m_MainThreadQueueMutex;

  private:
    friend int ::main(int argc, char **argv);
};

// To be defined in CLIENT
Application *CreateApplication();
} // namespace Engine
