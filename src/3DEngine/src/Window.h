#pragma once

#include <string>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Input/Input.h"

struct WindowProps
{
    std::string Title;
    uint32_t Width;
    uint32_t Height;
    WindowEventState EventState;

    WindowProps(const std::string &title = "3D Engine", uint32_t width = 1600, uint32_t height = 900)
        : Title(title), Width(width), Height(height), EventState(WindowEventState::None)
    {
    }
};

class Window
{
  public:
    Window() = default;
    Window(const WindowProps &props = WindowProps());
    virtual ~Window();

    void OnUpdate();

    GLFWwindow *GetNativeWindow() const { return m_Window; }
    Input GetInput() const { return m_Input; }

  private:
    void Init(const WindowProps &props);
    void SetInputEventCallbacks();
    void SetWindowEventCallbacks();
    void Shutdown();

  private:
    GLFWwindow *m_Window;
    WindowProps m_WindowProps;
    Input m_Input{};
};