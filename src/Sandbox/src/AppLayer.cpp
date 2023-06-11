#include "AppLayer.h"

#include <imgui.h>
#include <iostream>

Engine::WindowState windowState = Engine::InputManager::Instance().GetWindowState();

AppLayer::AppLayer() {}

void AppLayer::OnAttach()
{
    m_ModelShader =
        std::make_unique<Engine::Shader>("/home/adeleye/Source/3DEngine/src/Sandbox/res/shaders/model.vert",
                                         "/home/adeleye/Source/3DEngine/src/Sandbox/res/shaders/model.frag");

    m_ModelShader->SetUniform4f("lightColor", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
    m_ModelShader->SetUniform3f("lightPos", glm::vec3(0.3f, 0.6f, 1.0f));

    m_Model =
        std::make_unique<Engine::Model>("/home/adeleye/Source/3DEngine/src/Sandbox/res/models/suzanne/scene.gltf");

    m_Framebuffer = std::make_shared<Engine::Framebuffer>(windowState.Width, windowState.Height);

    m_Camera.SetFieldOfView(80.0f);
}

void AppLayer::OnDetach() {}

void AppLayer::OnUpdate(float deltaTime)
{
    {
        m_Framebuffer->Bind();
        Engine::RendererCommand::SetClearColor({0.1f, 0.1f, 0.1f, 1.0f});
        Engine::RendererCommand::Clear();
    }
    // m_Model->Draw(*m_ModelShader, m_Camera);
    Engine::Renderer3D::BeginScene(m_Camera);
    Engine::Renderer3D::DrawModel(*m_Model, *m_ModelShader);
    Engine::Renderer3D::EndScene();

    m_Framebuffer->Unbind();
}

void AppLayer::OnImGuiRender()
{
    static bool dockspace_open = true;
    static bool opt_fullscreen = true;
    static bool opt_padding = false;
    static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

    // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
    // because it would be confusing to have two docking targets within each others.
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
    if (opt_fullscreen)
    {
        const ImGuiViewport *viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(viewport->WorkSize);
        ImGui::SetNextWindowViewport(viewport->ID);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
                        ImGuiWindowFlags_NoMove;
        window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
    }
    else
        dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;

    // When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
    // and handle the pass-thru hole, so we ask Begin() to not render a background.
    if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
        window_flags |= ImGuiWindowFlags_NoBackground;

    if (!opt_padding)
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("DockSpace Demo", &dockspace_open, window_flags);
    if (!opt_padding)
        ImGui::PopStyleVar();

    if (opt_fullscreen)
        ImGui::PopStyleVar(2);

    // Submit the DockSpace
    ImGuiIO &io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
    {
        ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
    }

    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Exit", ""))
                Engine::Application::Close();
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }

    ImGui::Begin("Camera");
    ImGui::Text("Camera Position: (%f, %f, %f)", m_Camera.GetPosition().x, m_Camera.GetPosition().y,
                m_Camera.GetPosition().z);
    // control camera properties
    float position = m_Camera.GetPosition().x;
    if (ImGui::SliderFloat("X", &position, -10.0f, 10.0f))
        m_Camera.SetPosition({position, m_Camera.GetPosition().y, m_Camera.GetPosition().z});
    position = m_Camera.GetPosition().y;
    if (ImGui::SliderFloat("Y", &position, -10.0f, 10.0f))
        m_Camera.SetPosition({m_Camera.GetPosition().x, position, m_Camera.GetPosition().z});
    position = m_Camera.GetPosition().z;
    if (ImGui::SliderFloat("Z", &position, -10.0f, 10.0f))
        m_Camera.SetPosition({m_Camera.GetPosition().x, m_Camera.GetPosition().y, position});
    float yaw = m_Camera.GetYaw();
    if (ImGui::SliderFloat("Yaw", &yaw, -180.0f, 180.0f))
        m_Camera.SetYaw(yaw);
    float pitch = m_Camera.GetPitch();
    if (ImGui::SliderFloat("Pitch", &pitch, -89.0f, 89.0f))
        m_Camera.SetPitch(pitch);
    ImGui::End();

    ImGui::Begin("Model");
    position = m_Model->GetPosition().x;
    if (ImGui::SliderFloat("X", &position, -10.0f, 10.0f))
        m_Model->SetPosition({position, m_Model->GetPosition().y, m_Model->GetPosition().z});
    position = m_Model->GetPosition().y;
    if (ImGui::SliderFloat("Y", &position, -10.0f, 10.0f))
        m_Model->SetPosition({m_Model->GetPosition().x, position, m_Model->GetPosition().z});
    position = m_Model->GetPosition().z;
    if (ImGui::SliderFloat("Z", &position, -10.0f, 10.0f))
        m_Model->SetPosition({m_Model->GetPosition().x, m_Model->GetPosition().y, position});
    ImGui::End();

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{0, 0});
    ImGui::Begin("Viewport");
    m_ViewportFocused = ImGui::IsWindowFocused();
    auto viewportPanelSize = ImGui::GetContentRegionAvail();
    if (m_ViewportSize != *((glm::vec2 *)&viewportPanelSize))
    {
        m_Framebuffer->Resize((int)viewportPanelSize.x, (int)viewportPanelSize.y);
        m_ViewportSize = {viewportPanelSize.x, viewportPanelSize.y};
    }
    ImGui::Image((void *)(intptr_t)m_Framebuffer->GetColorAttachment(), ImVec2{m_ViewportSize.x, m_ViewportSize.y});
    ImGui::End();
    ImGui::PopStyleVar();

    ImGui::End();
}

void AppLayer::OnKeyPressed(Engine::InputKey key, bool isRepeat) {}

void AppLayer::OnMouseMoved(double xPos, double yPos, double xOffset, double yOffset) {}

void AppLayer::OnMouseButtonPressed(Engine::MouseButton button) {}

void AppLayer::OnWindowResize(int width, int height) {}
