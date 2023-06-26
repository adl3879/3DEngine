#include "AppLayer.h"

#include <imgui.h>
#include <iostream>

namespace Engine
{
WindowState windowState = InputManager::Instance().GetWindowState();

AppLayer::AppLayer() {}

void AppLayer::OnAttach()
{
    m_Framebuffer = std::make_shared<Engine::Framebuffer>(windowState.Width, windowState.Height);

    m_Scene = std::make_shared<Scene>();
    m_SceneHierarchyPanel.SetContext(m_Scene);

    LOG_INFO("AppLayer Attached");
}

void AppLayer::OnDetach() {}

void AppLayer::OnUpdate(float deltaTime)
{
    {
        m_Framebuffer->Bind();
        RendererCommand::SetClearColor({0.1f, 0.1f, 0.1f, 1.0f});
        RendererCommand::Clear();
    }

    Renderer3D::BeginScene(*m_Scene->GetSceneCamera());
    Renderer3D::DrawSkybox();
    Renderer3D::EndScene();

    m_Scene->OnUpdate(deltaTime);

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
    if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode) window_flags |= ImGuiWindowFlags_NoBackground;

    if (!opt_padding) ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("DockSpace Demo", &dockspace_open, window_flags);
    if (!opt_padding) ImGui::PopStyleVar();

    if (opt_fullscreen) ImGui::PopStyleVar(2);

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
            if (ImGui::MenuItem("New", "Ctrl+N"))
            {
                m_Scene = std::make_unique<Scene>();
                m_SceneHierarchyPanel.SetContext(m_Scene);
            }
            if (ImGui::MenuItem("Open...", "Ctrl+O"))
            {
                Utils::FileDialogs::OpenFile(Utils::FileDialogParams{
                    .DefaultPathAndFile = "/home/adeleye/Source/3DEngine/src/Sandbox/res/scenes/scene1.scene",
                    .SingleFilterDescription = "Scene Files (*.scene)\0*.scene\0"});
            }
            if (ImGui::MenuItem("Save As...", "Ctrl+Shift+S"))
            {
                Utils::FileDialogs::SaveFile(Utils::FileDialogParams{
                    .DefaultPathAndFile = "/home/adeleye/Source/3DEngine/src/Sandbox/res/scenes/scene1.scene",
                    .SingleFilterDescription = "Scene Files (*.scene)\0*.scene\0"});
            }
            if (ImGui::MenuItem("Exit", "")) Application::Close();

            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }

    if (Utils::FileDialogs::FileIsOpened())
    {
        // serialize the scene
        LOG_INFO("Serializing Scene");
        m_Scene = std::make_unique<Scene>();
        m_SceneHierarchyPanel.SetContext(m_Scene);

        SceneSerializer serializer(m_Scene);
        serializer.Deserialize(Utils::FileDialogs::m_SelectedFile);
    }

    if (Utils::FileDialogs::FileIsSaved())
    {
        // serialize the scene
        LOG_INFO("Serializing Scene");

        SceneSerializer serializer(m_Scene);
        serializer.Serialize(Utils::FileDialogs::m_SavedFile);
    }

    m_SceneHierarchyPanel.OnImGuiRender();

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

void AppLayer::OnKeyPressed(InputKey key, bool isRepeat) {}

void AppLayer::OnMouseMoved(double xPos, double yPos, double xOffset, double yOffset) {}

void AppLayer::OnMouseButtonPressed(MouseButton button) {}

void AppLayer::OnWindowResize(int width, int height) {}
} // namespace Engine