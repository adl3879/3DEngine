#include "AppLayer.h"

#include <imgui.h>
#include <iostream>

Engine::WindowState windowState = Engine::InputManager::Instance().GetWindowState();

AppLayer::AppLayer() {}

void AppLayer::OnAttach()
{
    m_Light = std::make_unique<Engine::Light>(Engine::LightType::Directional);

    m_ModelShader =
        std::make_unique<Engine::Shader>("/home/adeleye/Source/3DEngine/src/Sandbox/res/shaders/model.vert",
                                         "/home/adeleye/Source/3DEngine/src/Sandbox/res/shaders/model.frag");

    m_Model =
        std::make_unique<Engine::Model>("/home/adeleye/Source/3DEngine/src/Sandbox/res/models/suzanne/scene.gltf");

    m_Framebuffer = std::make_shared<Engine::Framebuffer>(windowState.Width, windowState.Height);

    m_Camera.SetFieldOfView(80.0f);

    m_Light->SetPosition(glm::vec3(0.3f, 0.6f, 1.0f));
}

void AppLayer::OnDetach() {}

void AppLayer::OnUpdate(float deltaTime)
{
    {
        m_Framebuffer->Bind();
        Engine::RendererCommand::SetClearColor({0.1f, 0.1f, 0.1f, 1.0f});
        Engine::RendererCommand::Clear();
    }
    Engine::Renderer3D::BeginScene(m_Camera, *m_Light);
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
    auto position = m_Camera.GetPosition();
    if (ImGui::SliderFloat3("Position", (float *)&position, -10.0f, 10.0f))
        m_Camera.SetPosition(position);
    float yaw = m_Camera.GetYaw();
    if (ImGui::SliderFloat("Yaw", &yaw, -180.0f, 180.0f))
        m_Camera.SetYaw(yaw);
    float pitch = m_Camera.GetPitch();
    if (ImGui::SliderFloat("Pitch", &pitch, -89.0f, 89.0f))
        m_Camera.SetPitch(pitch);
    ImGui::End();

    ImGui::Begin("Model");
    auto modelPosition = m_Model->GetPosition();
    if (ImGui::SliderFloat3("Position", (float *)&modelPosition, -10.0f, 10.0f))
        m_Model->SetPosition(modelPosition);
    float rotation = m_Model->GetRotation().x;
    if (ImGui::SliderFloat("Rotation X", &rotation, -180.0f, 180.0f))
        m_Model->SetRotation({rotation, m_Model->GetRotation().y, m_Model->GetRotation().z});
    rotation = m_Model->GetRotation().y;
    if (ImGui::SliderFloat("Rotation Y", &rotation, -180.0f, 180.0f))
        m_Model->SetRotation({m_Model->GetRotation().x, rotation, m_Model->GetRotation().z});
    rotation = m_Model->GetRotation().z;
    if (ImGui::SliderFloat("Rotation Z", &rotation, -180.0f, 180.0f))
        m_Model->SetRotation({m_Model->GetRotation().x, m_Model->GetRotation().y, rotation});
    float scale = m_Model->GetScale().x;
    if (ImGui::SliderFloat("Scale X", &scale, 0.0f, 10.0f))
        m_Model->SetScale({scale, scale, scale});
    ImGui::End();

    ImGui::Begin("Lighting");
    // select light type
    const char *lightTypes[] = {"Point", "Spot", "Directional", "None"};
    static int lightType = 0;
    if (ImGui::BeginCombo("Light Type", lightTypes[lightType]))
    {
        for (int i = 0; i < 3; i++)
        {
            bool isSelected = (lightType == i);
            if (ImGui::Selectable(lightTypes[i], isSelected))
            {
                lightType = i;
                m_Light->SetType(lightType);
            }
            if (isSelected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }

    if (lightType == 0)
    {
        ImGui::Text("Point Light");
        ImGui::Separator();
        // control point light properties
        auto lightPos = m_Light->GetPosition();
        if (ImGui::SliderFloat3("Light Position", (float *)&lightPos, -10.0f, 10.0f))
            m_Light->SetPosition(lightPos);
        float constant = m_Light->PointLightProps.Constant;
        if (ImGui::SliderFloat("Constant", &constant, 0.0f, 1.0f))
            m_Light->PointLightProps.Constant = constant;
        float linear = m_Light->PointLightProps.Linear;
        if (ImGui::SliderFloat("Linear", &linear, 0.0f, 1.0f))
            m_Light->PointLightProps.Linear = linear;
        float quadratic = m_Light->PointLightProps.Quadratic;
        if (ImGui::SliderFloat("Quadratic", &quadratic, 0.0f, 1.0f))
            m_Light->PointLightProps.Quadratic = quadratic;
        float ambient = m_Light->PointLightProps.Ambient;
        if (ImGui::SliderFloat("Ambient", &ambient, 0.0f, 1.0f))
            m_Light->PointLightProps.Ambient = ambient;
        float specular = m_Light->PointLightProps.Specular;
        if (ImGui::SliderFloat("Specular", &specular, 0.0f, 1.0f))
            m_Light->PointLightProps.Specular = specular;
        auto color = m_Light->GetColor();
        if (ImGui::ColorEdit3("Color", (float *)&color))
            m_Light->SetColor(color);
    }
    if (lightType == 1)
    {
        ImGui::Text("Spot Light");
        ImGui::Separator();
        // control spot light properties
        auto lightPos = m_Light->GetPosition();
        if (ImGui::SliderFloat3("Light Position", (float *)&lightPos, -10.0f, 10.0f))
            m_Light->SetPosition(lightPos);
        float cutOff = m_Light->SpotLightProps.CutOff;
        if (ImGui::SliderFloat("Cut Off", &cutOff, 0.0f, 1.0f))
            m_Light->SpotLightProps.CutOff = cutOff;
        float outerCutOff = m_Light->SpotLightProps.OuterCutOff;
        if (ImGui::SliderFloat("Outer Cut Off", &outerCutOff, 0.0f, 1.0f))
            m_Light->SpotLightProps.OuterCutOff = outerCutOff;
        float ambient = m_Light->SpotLightProps.Ambient;
        if (ImGui::SliderFloat("Ambient", &ambient, 0.0f, 1.0f))
            m_Light->SpotLightProps.Ambient = ambient;
        float specular = m_Light->SpotLightProps.Specular;
        if (ImGui::SliderFloat("Specular", &specular, 0.0f, 1.0f))
            m_Light->SpotLightProps.Specular = specular;
        auto color = m_Light->GetColor();
        if (ImGui::ColorEdit3("Color", (float *)&color))
            m_Light->SetColor(color);
    }
    if (lightType == 2)
    {
        ImGui::Text("Directional Light");
        ImGui::Separator();
        // control directional light properties
        auto direction = m_Light->DirectionalLightProps.Direction;
        if (ImGui::SliderFloat3("Direction", (float *)&direction, -1.0f, 1.0f))
            m_Light->DirectionalLightProps.Direction = direction;
        float ambient = m_Light->DirectionalLightProps.Ambient;
        if (ImGui::SliderFloat("Ambient", &ambient, 0.0f, 1.0f))
            m_Light->DirectionalLightProps.Ambient = ambient;
        float specular = m_Light->DirectionalLightProps.Specular;
        if (ImGui::SliderFloat("Specular", &specular, 0.0f, 1.0f))
            m_Light->DirectionalLightProps.Specular = specular;
        auto color = m_Light->GetColor();
        if (ImGui::ColorEdit3("Color", (float *)&color))
            m_Light->SetColor(color);
    }
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
