#include "Application.h"
#include "Buffer.h"
#include "GLFW/glfw3.h"
#include "Input/InputDevice.h"
#include "Input/InputKey.h"
#include "Input/InputManager.h"

#include <functional>
#include <iostream>
#include <memory>

float lastFrame = 0.0f;

Application::Application() : m_IsRunning(true)
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    m_Window = glfwCreateWindow(windowWidth, windowHeight, "LearnOpenGL", nullptr, nullptr);
    if (m_Window == nullptr)
    {
        std::cout << "Failed to create a GLFW window" << std::endl;
        glfwTerminate();
    }
    glfwMakeContextCurrent(m_Window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
    }

    SetupInputSystem();

    // Define the viewport dimensions
    int width, height;
    glfwGetFramebufferSize(m_Window, &width, &height);
    glViewport(0, 0, width, height);

    // Setup OpenGL options
    glEnable(GL_DEPTH_TEST);

    m_LightShader = std::make_unique<Shader>("../res/shaders/light.vert", "../res/shaders/light.frag");
    m_ModelShader = std::make_unique<Shader>("../res/shaders/model.vert", "../res/shaders/model.frag");

    m_LightShader->Use();
    m_LightShader->SetUniform4f("lightColor", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
    // coordinate systems
    glm::mat4 model{1.0f};
    glm::vec3 lightPos = glm::vec3(0.0f, 0.6f, 0.0f);
    model = glm::translate(model, lightPos);
    model = glm::scale(model, glm::vec3(0.8f));
    m_LightShader->SetUniformMatrix4fv("model", model);

    m_ModelShader->Use();
    glm::mat4 model3{1.0f};
    m_ModelShader->SetUniformMatrix4fv("model", model3);
    m_ModelShader->SetUniform4f("lightColor", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
    m_ModelShader->SetUniform3f("lightPos", glm::vec3(0.0f, 0.6f, 0.0f));
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
    while (m_IsRunning && !glfwWindowShouldClose(m_Window))
    {
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
        glfwSwapBuffers(m_Window);
    }
}

void Application::SetupInputSystem()
{
    InputManager::Instance().RegisterKeyboardCallback(
        [](InputKey key, float isRepeat)
        {
            if (key == InputKey::Space)
                std::cout << "Hello form Space :)\n";
        });

    InputManager::Instance().MapInputToAction(InputKey::Escape, InputAction{.ActionName = "quit", .Scale = 1.0f});
    InputManager::Instance().RegisterActionCallback(
        "quit", InputManager::ActionCallback{.Ref = "quit",
                                             .Func = [this](InputSource source, int value, float scale)
                                             {
                                                 m_IsRunning = false;
                                                 return true;
                                             }});

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
}

Application::~Application() { glfwTerminate(); }
