#include "AppLayer.h"

#include <iostream>

AppLayer::AppLayer() {}

void AppLayer::OnAttach()
{
    m_ModelShader =
        std::make_unique<Engine::Shader>("/home/adeleye/Source/3DEngine/src/Sandbox/res/shaders/model.vert",
                                         "/home/adeleye/Source/3DEngine/src/Sandbox/res/shaders/model.frag");

    m_ModelShader->Use();
    glm::mat4 model3{1.0f};
    m_ModelShader->SetUniformMatrix4fv("model", model3);
    m_ModelShader->SetUniform4f("lightColor", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
    m_ModelShader->SetUniform3f("lightPos", glm::vec3(0.3f, 0.6f, 0.0f));
    m_ModelShader->SetUniform3f("cameraPos", m_Camera.GetPosition());

    m_Model =
        std::make_unique<Engine::Model>("/home/adeleye/Source/3DEngine/src/Sandbox/res/models/suzanne/scene.gltf");
}

void AppLayer::OnDetach() {}

void AppLayer::OnUpdate(float deltaTime)
{
    m_CameraController.OnUpdate(deltaTime);
    m_Model->Draw(*m_ModelShader, m_Camera);
}

void AppLayer::OnKeyPressed(Engine::InputKey key, bool isRepeat)
{
    if (key == Engine::InputKey::A)
        std::cout << "A" << std::endl;
}

void AppLayer::OnMouseMoved(double xPos, double yPos, double xOffset, double yOffset)
{
    // std::cout << "Mouse moved: " << xOffset << ", " << yOffset << std::endl;
}

void AppLayer::OnMouseButtonPressed(Engine::MouseButton button)
{
    if (button == Engine::MouseButton::Right)
        std::cout << "hello from right\n";
    else if (button == Engine::MouseButton::Left)
        std::cout << "hello from left\n";
}

void AppLayer::OnWindowResize(int width, int height)
{
    // std::cout << "Window resized: " << width << ", " << height << std::endl;
}
