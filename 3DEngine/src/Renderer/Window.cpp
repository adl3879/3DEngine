#include "Window.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "InputManager.h"
#include "Log.h"
#include <iostream>

namespace Engine
{
Window::Window(const WindowProps &props) : m_WindowProps(props)
{
    Init(props);

    // Register input manager
    InputManager::Instance().RegisterDevice(InputDevice{
        .Type = InputDeviceType::Keyboard,
        .Index = 0,
        .KeyboardStateFunc = std::bind(&Input::GetKeyboardState, &m_Input, std::placeholders::_1),
    });
    InputManager::Instance().RegisterDevice(InputDevice{
        .Type = InputDeviceType::Mouse,
        .Index = 0,
        .MousePressStateFunc = std::bind(&Input::GetMousePressState, &m_Input, std::placeholders::_1),
    });
    InputManager::Instance().RegisterDevice(InputDevice{
        .Type = InputDeviceType::MouseMove,
        .Index = 0,
        .CursorStateFunc = std::bind(&Input::GetCursorPosition, &m_Input, std::placeholders::_1),
    });
    InputManager::Instance().RegisterDevice(InputDevice{
        .Type = InputDeviceType::Window,
        .Index = 0,
        .WindowStateFunc = std::bind(&Input::GetWindowState, &m_Input, std::placeholders::_1),
    });
    InputManager::Instance().RegisterDevice(InputDevice{
        .Type = InputDeviceType::MouseScroll,
        .Index = 0,
        .MouseScrollStateFunc = std::bind(&Input::GetMouseScrollState, &m_Input, std::placeholders::_1),
    });
}

Window::~Window() { Shutdown(); }

void Window::OnUpdate()
{
    // Define the viewport dimensions
    auto windowState = InputManager::Instance().GetWindowState();
    glViewport(0, 0, windowState.Width, windowState.Height);

    // reset mouse scroll state
    m_Input.UpdateMouseScrollState(0.0f, 0.0f);

    glfwPollEvents();
}

void Window::Init(const WindowProps &props)
{
    glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

    GLFWmonitor *MyMonitor = glfwGetPrimaryMonitor(); // The primary monitor.. Later Occulus?..

    const GLFWvidmode *mode = glfwGetVideoMode(MyMonitor);
    auto SCR_WIDTH = mode->width;
    auto SCR_HEIGHT = mode->height;

    m_Window = glfwCreateWindow(m_WindowProps.Width, m_WindowProps.Height, props.Title.c_str(), nullptr, nullptr);
    if (m_Window == nullptr)
    {
        LOG_CORE_ERROR("Failed to create a GLFW window");
        glfwTerminate();
    }
    glfwMakeContextCurrent(m_Window);

    SetWindowEventCallbacks();
    SetInputEventCallbacks();

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        LOG_CORE_ERROR("Failed to initialize GLAD");
    }
    // Setup OpenGL options
    glEnable(GL_DEPTH_TEST);
}

void Window::SetInputEventCallbacks()
{
    glfwSetWindowUserPointer(m_Window, &m_Input);

    auto keyCallback = [](GLFWwindow *window, int key, int scancode, int action, int mods)
    {
        Input *input = static_cast<Input *>(glfwGetWindowUserPointer(window));

        float value = 0.0f;
        switch (action)
        {
            case GLFW_PRESS: value = 1.0f; break;
            case GLFW_RELEASE: value = -1.0f; break;
            case GLFW_REPEAT: value = 2.0f; break;
        };
        input->UpdateKeyboardState(key, value);
    };
    glfwSetKeyCallback(m_Window, keyCallback);

    auto mouseCallback = [](GLFWwindow *window, int button, int action, int mods)
    {
        Input *input = static_cast<Input *>(glfwGetWindowUserPointer(window));

        float value = 0.0f;
        switch (action)
        {
            case GLFW_PRESS: value = 1.0f; break;
            case GLFW_RELEASE: value = -1.0f; break;
        };
        input->UpdateMousePressState(button, value);
    };
    glfwSetMouseButtonCallback(m_Window, mouseCallback);

    auto cursorPositionCallback = [](GLFWwindow *window, double xPos, double yPos)
    {
        Input *input = static_cast<Input *>(glfwGetWindowUserPointer(window));
        input->UpdateCursorPosition(xPos, yPos);
    };
    glfwSetCursorPosCallback(m_Window, cursorPositionCallback);

    auto scrollCallback = [](GLFWwindow *window, double xOffset, double yOffset)
    {
        Input *input = static_cast<Input *>(glfwGetWindowUserPointer(window));
        input->UpdateMouseScrollState(xOffset, yOffset);
    };
    glfwSetScrollCallback(m_Window, scrollCallback);
}

void Window::SetWindowEventCallbacks()
{
    glfwSetWindowUserPointer(m_Window, &m_Input);

    auto windowCloseCallback = [](GLFWwindow *window)
    {
        Input *input = static_cast<Input *>(glfwGetWindowUserPointer(window));
        input->UpdateWindowState(WindowState{
            .EventState = WindowEventState::Close,
        });
    };
    glfwSetWindowCloseCallback(m_Window, windowCloseCallback);

    auto windowResizeCallback = [](GLFWwindow *window, int width, int height)
    {
        Input *input = static_cast<Input *>(glfwGetWindowUserPointer(window));
        input->UpdateWindowState(WindowState{
            .EventState = WindowEventState::Close,
            .Width = width,
            .Height = height,
        });
    };
    glfwSetWindowSizeCallback(m_Window, windowResizeCallback);
}

void Window::Shutdown()
{
    glfwDestroyWindow(m_Window);
    glfwTerminate();
}
} // namespace Engine
