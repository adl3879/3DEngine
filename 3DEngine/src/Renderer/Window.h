#pragma once

#include <string>

#include "Input.h"

struct GLFWwindow;

namespace Engine
{
constexpr uint32_t WINDOW_WIDTH = 2600;
constexpr uint32_t WINDOW_HEIGHT = 1600;

struct WindowProps
{
    std::string Title;
    uint32_t Width;
    uint32_t Height;
    WindowEventState EventState;

    WindowProps(const std::string &title = "3D Engine", uint32_t width = WINDOW_WIDTH, uint32_t height = WINDOW_HEIGHT)
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
} // namespace Engine
