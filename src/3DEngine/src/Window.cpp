#include "Window.h"
#include "InputManager.h"

#include <iostream>

Window::Window(const WindowProps &props) : m_WindowProps(props)
{
    //
    Init(props);

    // Register input manager
    InputManager::Instance().RegisterDevice(InputDevice{
        .Type = InputDeviceType::Keyboard,
        .Index = 0,
        .StateFunc = std::bind(&Input::GetKeyboardState, &m_Input, std::placeholders::_1),
    });
    InputManager::Instance().RegisterDevice(InputDevice{
        .Type = InputDeviceType::Mouse,
        .Index = 0,
        .StateFunc = std::bind(&Input::GetMouseState, &m_Input, std::placeholders::_1),
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
}

Window::~Window() { Shutdown(); }

void Window::OnUpdate()
{
    // Define the viewport dimensions
    auto windowState = InputManager::Instance().GetWindowState();
    glViewport(0, 0, windowState.Width, windowState.Height);
    glfwPollEvents();

    glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Window::Init(const WindowProps &props)
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

    m_Window = glfwCreateWindow(props.Width, props.Height, props.Title.c_str(), nullptr, nullptr);
    if (m_Window == nullptr)
    {
        std::cout << "Failed to create a GLFW window" << std::endl;
        glfwTerminate();
    }
    glfwMakeContextCurrent(m_Window);

    SetWindowEventCallbacks();
    SetInputEventCallbacks();
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
        case GLFW_PRESS:
        case GLFW_REPEAT:
            value = 1.0f;
            break;
        default:
            value = 0.0f;
            break;
        };
        input->UpdateKeyboardState(key, value);
    };
    glfwSetKeyCallback(m_Window, keyCallback);

    auto mouseCallback = [](GLFWwindow *window, int button, int action, int mods)
    {
        Input *input = static_cast<Input *>(glfwGetWindowUserPointer(window));
        if (input)
        {
            input->UpdateMouseState(button, action == GLFW_PRESS ? 1.0f : 0.0f);
        }
    };
    glfwSetMouseButtonCallback(m_Window, mouseCallback);

    auto cursorPositionCallback = [](GLFWwindow *window, double xPos, double yPos)
    {
        Input *input = static_cast<Input *>(glfwGetWindowUserPointer(window));
        input->UpdateCursorPosition(xPos, yPos);
    };
    glfwSetCursorPosCallback(m_Window, cursorPositionCallback);
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
