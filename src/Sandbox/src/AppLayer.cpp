#include "AppLayer.h"

#include <imgui.h>
#include <iostream>
#include <filesystem>

#include <ImGuizmo.h>
#include "Math.h"
#include "Light.h"

namespace Engine
{
WindowState windowState = InputManager::Instance().GetWindowState();

AppLayer::AppLayer() : m_EditorCamera(-45.0f, 1.778f, 0.1f, 100.0f) {}

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
    // update
    m_EditorCamera.OnUpdate(deltaTime);
    m_EditorCamera.SetViewportSize(m_ViewportSize.x, m_ViewportSize.y);

    {
        m_Framebuffer->Bind();
        RendererCommand::SetClearColor({0.0f, 0.0f, 0.0f, 1.0f});
        RendererCommand::Clear();
    }

    // if (m_Scene->GetMainCamera() != nullptr)
    //     Renderer3D::BeginScene(*m_Scene->GetMainCamera());
    // else
    //     Renderer3D::BeginScene(m_EditorCamera);
    // Renderer3D::DrawSkybox();
    // Renderer3D::EndScene();

    m_Scene->OnUpdateEditor(deltaTime, m_EditorCamera);

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
            if (ImGui::MenuItem("New", "Ctrl+N")) NewScene();
            if (ImGui::MenuItem("Open...", "Ctrl+O")) OpenScene();
            if (ImGui::MenuItem("Save As...", "Ctrl+Shift+S")) SaveSceneAs();
            if (ImGui::MenuItem("Exit", "")) Application::Close();

            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }

    if (Utils::FileDialogs::FileIsOpened("openScene"))
    {
        // serialize the scene
        LOG_INFO("Serializing Scene");
        m_Scene = std::make_unique<Scene>();
        m_SceneHierarchyPanel.SetContext(m_Scene);

        SceneSerializer serializer(m_Scene);
        serializer.Deserialize(Utils::FileDialogs::m_SelectedFile);
    }

    if (Utils::FileDialogs::FileIsSaved("saveScene"))
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

    // Gizmos
    auto selectedEntity = m_SceneHierarchyPanel.GetSelectedEntity();

    if (selectedEntity && m_GizmoType != -1)
    {
        ImGuizmo::SetOrthographic(false);
        ImGuizmo::SetDrawlist();
        ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, m_ViewportSize.x, m_ViewportSize.y);

        glm::mat4 cameraView = m_EditorCamera.GetViewMatrix();
        auto projection = m_EditorCamera.GetProjectionMatrix();

        cameraView[0][1] = -cameraView[0][1];
        cameraView[1][1] = -cameraView[1][1];
        cameraView[2][1] = -cameraView[2][1];

        // Entity Transform
        if (selectedEntity.HasComponent<TransformComponent>())
        {
            auto &tc = selectedEntity.GetComponent<TransformComponent>();
            glm::mat4 transform = tc.GetTransform();

            // Snapping
            bool snap = InputManager::Instance().IsKeyPressed(InputKey::LeftControl);
            float snapValue = m_GizmoType == ImGuizmo::OPERATION::ROTATE ? 45.0f : 0.5f;
            float snapValues[3] = {snapValue, snapValue, snapValue};

            ImGuizmo::Manipulate(glm::value_ptr(cameraView), glm::value_ptr(projection),
                                 (ImGuizmo::OPERATION)m_GizmoType, ImGuizmo::MODE::LOCAL, glm::value_ptr(transform),
                                 nullptr, snap ? snapValues : nullptr);

            if (ImGuizmo::IsUsing())
            {
                glm::vec3 translation, rotation, scale;
                Math::DecomposeTransform(transform, translation, rotation, scale);
                glm::vec3 deltaRotation = rotation - tc.Rotation;

                tc.Translation = translation;
                tc.Rotation += deltaRotation;
                tc.Scale = scale;
            }
        }
    }

    ImGui::End();
    ImGui::PopStyleVar();

    // ImGui::ShowDemoWindow();

    ImGui::End();
}

void AppLayer::OnKeyPressed(InputKey key, bool isRepeat)
{
    switch (key)
    {
        case InputKey::Q: m_GizmoType = -1; break;
        case InputKey::W: m_GizmoType = ImGuizmo::OPERATION::TRANSLATE; break;
        case InputKey::E: m_GizmoType = ImGuizmo::OPERATION::ROTATE; break;
        case InputKey::R: m_GizmoType = ImGuizmo::OPERATION::SCALE; break;
        default: break;
    }
}

void AppLayer::OnMouseScrolled(double xOffset, double yOffset) { m_EditorCamera.OnMouseScrolled(xOffset, yOffset); }

void AppLayer::NewScene()
{
    Light::Reset();
    m_Scene = std::make_unique<Scene>();
    m_SceneHierarchyPanel.SetContext(m_Scene);
}

void AppLayer::OpenScene()
{
    Light::Reset();
    Utils::FileDialogs::OpenFile(
        "openScene", Utils::FileDialogParams{.DefaultPathAndFile =
                                                 "/home/adeleye/Source/3DEngine/src/Sandbox/res/scenes/scene1.scene",
                                             .SingleFilterDescription = "Scene Files (*.scene)\0*.scene\0"});
}

void AppLayer::SaveSceneAs()
{
    Utils::FileDialogs::SaveFile(
        "saveScene", Utils::FileDialogParams{.DefaultPathAndFile =
                                                 "/home/adeleye/Source/3DEngine/src/Sandbox/res/scenes/scene1.scene",
                                             .SingleFilterDescription = "Scene Files (*.scene)\0*.scene\0"});
}
} // namespace Engine