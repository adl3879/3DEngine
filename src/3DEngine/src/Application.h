#pragma once

#include <memory>

#include "Window.h"
#include "LayerStack.h"

#include "ImGuiLayer.h"

int main(int argc, char **argv);

class Application
{
  public:
    Application();
    Application(Application const &app) = delete;
    Application &operator=(Application const &app) = delete;
    virtual ~Application();

    void PushLayer(Layer *layer);
    void PushOverlay(Layer *layer);

    void Close() { m_IsRunning = false; }

    bool IsRunning() const { return m_IsRunning; }
    float GetDeltaTime() const { return m_DeltaTime; }

    // GLFWwindow *GetNativeWindow() const { return m_Window->GetNativeWindow(); }
    const std::shared_ptr<Window> &GetWindow() const { return m_Window; }

  private:
    void Run();
    void SetupInputSystem();

  private:
    std::shared_ptr<Window> m_Window;

    bool m_IsRunning;
    float m_DeltaTime = 0.0f;

    LayerStack m_LayerStack{};
    ImGuiLayer *m_ImGuiLayer;

  private:
    friend int ::main(int argc, char **argv);
};

// To be defined in CLIENT
Application *CreateApplication();