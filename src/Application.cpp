#include "Application.h"
#include "GLFW/glfw3.h"
#include "Input/InputDevice.h"
#include "Input/InputKey.h"
#include "Input/InputManager.h"

#include <functional>
#include <iostream>

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

    m_Shader = std::make_unique<Shader>("../res/shaders/vertex.glsl", "../res/shaders/fragment.glsl");
    m_Texture = std::make_unique<Texture>("../res/textures/wall.jpg");

    float vertices[] = {
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 0.5f,  -0.5f, -0.5f, 1.0f, 0.0f, 0.5f,  0.5f,  -0.5f, 1.0f, 1.0f,
        0.5f,  0.5f,  -0.5f, 1.0f, 1.0f, -0.5f, 0.5f,  -0.5f, 0.0f, 1.0f, -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,
        -0.5f, -0.5f, 0.5f,  0.0f, 0.0f, 0.5f,  -0.5f, 0.5f,  1.0f, 0.0f, 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 1.0f, -0.5f, 0.5f,  0.5f,  0.0f, 1.0f, -0.5f, -0.5f, 0.5f,  0.0f, 0.0f,
        -0.5f, 0.5f,  0.5f,  1.0f, 0.0f, -0.5f, 0.5f,  -0.5f, 1.0f, 1.0f, -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, -0.5f, -0.5f, 0.5f,  0.0f, 0.0f, -0.5f, 0.5f,  0.5f,  1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.5f,  0.5f,  -0.5f, 1.0f, 1.0f, 0.5f,  -0.5f, -0.5f, 0.0f, 1.0f,
        0.5f,  -0.5f, -0.5f, 0.0f, 1.0f, 0.5f,  -0.5f, 0.5f,  0.0f, 0.0f, 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.5f,  -0.5f, -0.5f, 1.0f, 1.0f, 0.5f,  -0.5f, 0.5f,  1.0f, 0.0f,
        0.5f,  -0.5f, 0.5f,  1.0f, 0.0f, -0.5f, -0.5f, 0.5f,  0.0f, 0.0f, -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        -0.5f, 0.5f,  -0.5f, 0.0f, 1.0f, 0.5f,  0.5f,  -0.5f, 1.0f, 1.0f, 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f, -0.5f, 0.5f,  0.5f,  0.0f, 0.0f, -0.5f, 0.5f,  -0.5f, 0.0f, 1.0f};
    GLuint indices[] = {
        0, 1, 3, // First Triangle
        1, 2, 3  // Second Triangle
    };

    m_VAO = std::make_unique<VertexArray>();
    m_VBO = std::make_unique<VertexBuffer>(vertices, sizeof(vertices));
    m_EBO = std::make_unique<IndexBuffer>(indices, sizeof(indices));

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid *)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid *)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);

    m_VAO->Unbind();
}

Application &Application::Instance()
{
    static Application app;
    return app;
}

int num = 0;

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

    auto cursorPositionCallback = [](GLFWwindow *window, double xpos, double ypos)
    {
        Input *input = static_cast<Input *>(glfwGetWindowUserPointer(window));
        input->UpdateCursorPosition(xpos, ypos);
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

void Application::Run()
{
    while (m_IsRunning && !glfwWindowShouldClose(m_Window))
    {
        glfwPollEvents();
        // doMovement();
        InputManager::Instance().ProcessInput();

        m_CameraController.OnUpdate(m_DeltaTime);

        // delta time
        float currentFrame = glfwGetTime();
        m_DeltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        m_Texture->Bind();
        m_Shader->Use();
        // coordinate systems
        glm::mat4 model{1.0f};
        glm::mat4 projection{1.0f};

        model = glm::rotate(model, 2.0f, glm::vec3(0.5f, 1.0f, 0.0f));
        projection = glm::perspective(45.0f, (GLfloat)windowWidth / (GLfloat)windowHeight, 0.1f, 100.0f);

        m_Shader->SetUniformMatrix4fv("model", model);
        m_Shader->SetUniformMatrix4fv("view", m_Camera.GetViewMatrix());
        m_Shader->SetUniformMatrix4fv("projection", projection);

        m_VAO->Bind();
        glDrawArrays(GL_TRIANGLES, 0, 36);
        m_VAO->Unbind();

        // Swap the screen buffers
        glfwSwapBuffers(m_Window);
    }
}

Application::~Application() { glfwTerminate(); }
