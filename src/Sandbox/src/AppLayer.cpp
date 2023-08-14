#include "AppLayer.h"

#include <imgui.h>
#include <iostream>
#include <filesystem>

#include <ImGuizmo.h>
#include "Math.h"
#include "ResourceManager.h"
#include "AssetManager.h"

namespace Engine
{
WindowState windowState = InputManager::Instance().GetWindowState();

AppLayer::AppLayer() : m_EditorCamera(-45.0f, 1.778f, 0.1f, 100.0f) {}

void AppLayer::OnAttach()
{
    // TODO: load last opened project from a savefile
    Project::Load("/home/adeleye/Source/3DEngine/src/Sandbox/SandboxProject/SandboxProject.3dproj");

    auto fbSpec = FramebufferSpecification{};
    fbSpec.Attachments = {FramebufferTextureFormat::RGBA8, FramebufferTextureFormat::RED_INTEGER,
                          FramebufferTextureFormat::Depth};
    fbSpec.Width = windowState.Width;
    fbSpec.Height = windowState.Height;

    m_Framebuffer = std::make_shared<Engine::Framebuffer>(fbSpec);

    m_Scene = std::make_shared<Scene>();
    m_SceneHierarchyPanel.SetContext(m_Scene);
    // attach scene
    m_Scene->OnAttach();

    m_RenderSystem = std::make_shared<RenderSystem>();
    m_RenderSystem->Init();

    auto basePath = "/home/adeleye/Source/3DEngine/src/Sandbox/Resources/Icons";
    m_IconPlay = ResourceManager::Instance().LoadTexture(basePath + std::string("/PlayButton.png"));
    m_IconStop = ResourceManager::Instance().LoadTexture(basePath + std::string("/StopButton.png"));

    m_ContentBrowserPanel = std::make_shared<ContentBrowserPanel>();

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
        m_RenderSystem->Render(m_EditorCamera, *m_Scene);
        auto selectedEntity = m_SceneHierarchyPanel.GetSelectedEntity();
    }

    if (m_SceneState == SceneState::Edit)
        m_Scene->OnUpdateEditor(deltaTime, m_EditorCamera);
    else if (m_SceneState == SceneState::Play)
    {
        m_Scene->OnUpdate(deltaTime);
        m_Scene->OnUpdateRuntime(deltaTime);
    }

    auto [mx, my] = ImGui::GetMousePos();
    mx -= m_ViewportBounds[0].x;
    my -= m_ViewportBounds[0].y;
    glm::vec2 viewportSize = m_ViewportBounds[1] - m_ViewportBounds[0];

    int mouseX = (int)mx;
    int mouseY = (int)my;

    if (mouseX >= 0 && mouseY >= 0 && mouseX < (int)viewportSize.x && mouseY < (int)viewportSize.y)
    {
        auto pixelData = m_Framebuffer->ReadPixel(1, mouseX, mouseY);
        // LOG_CORE_TRACE("pixel {}", pixelData - 1);
        // removed one because i moved every entity by one
        m_HoveredEntity = pixelData == 0 ? Entity() : Entity((entt::entity)(pixelData - 1), m_Scene.get());
    }
    m_Framebuffer->Unbind();
}

void AppLayer::OnFixedUpdate(float dt)
{
    if (m_SceneState == SceneState::Play) m_Scene->OnFixedUpdate(dt);
}

void AppLayer::OnImGuiRender()
{
    static bool dockspace_open = true;
    static bool opt_fullscreen = true;
    static bool opt_padding = false;
    static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

    // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into
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
            ImGui::Separator();
            if (ImGui::MenuItem("New Project", "Ctrl+Shift+N")) NewProject();
            if (ImGui::MenuItem("Open Project", "Ctrl+Shift+O")) return;
            ImGui::Separator();
            if (ImGui::MenuItem("Save...", "Ctrl+S")) SaveScene();
            if (ImGui::MenuItem("Save As...", "Ctrl+Shift+S")) SaveSceneAs();
            ImGui::Separator();
            if (ImGui::MenuItem("Exit", "")) Application::Close();

            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }
    FileOperations();

    m_SceneHierarchyPanel.OnImGuiRender();
    m_MaterialEditorPanel.OnImGuiRender();
    m_ContentBrowserPanel->OnImGuiRender();

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{0, 0});
    ImGui::Begin("Viewport");
    auto viewportOffset = ImGui::GetCursorPos();

    m_ViewportFocused = ImGui::IsWindowFocused();
    m_ViewportHovered = ImGui::IsWindowHovered();
    auto viewportPanelSize = ImGui::GetContentRegionAvail();
    if (m_ViewportSize != *((glm::vec2 *)&viewportPanelSize))
    {
        m_Framebuffer->Resize((int)viewportPanelSize.x, (int)viewportPanelSize.y);
        m_ViewportSize = {viewportPanelSize.x, viewportPanelSize.y};
    }
    ImGui::Image((void *)(intptr_t)m_Framebuffer->GetColorAttachment(), ImVec2{m_ViewportSize.x, m_ViewportSize.y});

    if (ImGui::BeginDragDropTarget())
    {
        if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
        {
            const char *handle = (const char *)payload->Data;
            ResetScene("");
            m_Scene = AssetManager::GetAsset<Scene>(std::stoull(handle));
            m_SceneHierarchyPanel.SetContext(m_Scene);
        }
        ImGui::EndDragDropTarget();
    }

    auto windowSize = ImGui::GetWindowSize();
    auto miniBound = ImGui::GetWindowPos();
    miniBound.x += viewportOffset.x;
    miniBound.y += viewportOffset.y;

    auto maxBound = ImVec2(miniBound.x + windowSize.x, miniBound.y + windowSize.y);
    m_ViewportBounds[0] = {miniBound.x, miniBound.y};
    m_ViewportBounds[1] = {maxBound.x, maxBound.y};

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
    UI_Toolbar();

    ImGui::End();
}

void AppLayer::OnKeyPressed(InputKey key, bool isRepeat)
{
    auto Input = InputManager::Instance();

    auto ctrl = Input.IsKeyPressed(InputKey::LeftControl) || Input.IsKeyPressed(InputKey::RightControl);
    auto shift = Input.IsKeyPressed(InputKey::LeftShift) || Input.IsKeyPressed(InputKey::RightShift);

    switch (key)
    {
        case InputKey::Q: m_GizmoType = -1; break;
        case InputKey::W: m_GizmoType = ImGuizmo::OPERATION::TRANSLATE; break;
        case InputKey::E: m_GizmoType = ImGuizmo::OPERATION::ROTATE; break;
        case InputKey::R: m_GizmoType = ImGuizmo::OPERATION::SCALE; break;

        case InputKey::N:
            if (ctrl) NewScene();
            break;
        case InputKey::O:
            if (ctrl) OpenScene();
            break;
        case InputKey::S:
            if (ctrl && shift)
                SaveSceneAs();
            else if (ctrl)
                SaveScene();
            break;

        default: break;
    }
}

void AppLayer::OnMouseScrolled(double xOffset, double yOffset)
{
    if (m_ViewportFocused || m_ViewportHovered) m_EditorCamera.OnMouseScrolled(xOffset, yOffset);
}

void AppLayer::OnMouseButtonPressed(MouseButton button)
{
    auto Input = InputManager::Instance();
    // Mouse picking
    if (button == MouseButton::Left && !ImGuizmo::IsOver() && !Input.IsKeyPressed(InputKey::LeftAlt))
    {
        if (m_ViewportHovered) m_SceneHierarchyPanel.SetSelectedEntity(m_HoveredEntity);
    }
}

void AppLayer::NewProject()
{
    // Project::New()->SaveActive("/home/adeleye/Source/3DEngine/src/Sandbox/Untitled.3dproj");
    Utils::FileDialogs::CreateFolder("newProject",
                                     Utils::FileDialogParams{
                                         .Title = "New Project",
                                         .DefaultPathAndFile = "/home/adeleye/Source/3DEngine/src/SandboxProjects/",
                                     });
}

void AppLayer::OpenProject() {}

void AppLayer::NewScene()
{
    // TODO: create new scene file
    ResetScene("");
    m_Scene = std::make_unique<Scene>();
    m_SceneHierarchyPanel.SetContext(m_Scene);
}

void AppLayer::OpenScene()
{
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

void AppLayer::SaveScene()
{
    if (m_Scene->GetSceneFilePath().empty())
        SaveSceneAs();
    else
    {
        SceneSerializer serializer(m_Scene);
        serializer.Serialize(m_Scene->GetSceneFilePath());
    }
}

void AppLayer::ResetScene(const std::string &path)
{
    Light::Reset();
    m_EditorCamera = EditorCamera(-45.0f, 1.778f, 0.1f, 100.0f);
    // m_Scene = std::make_unique<Scene>();
    // m_Scene->SetSceneFilePath(path);
}

void AppLayer::OnScenePlay() { m_SceneState = SceneState::Play; }

void AppLayer::OnSceneStop() { m_SceneState = SceneState::Edit; }

void AppLayer::FileOperations()
{
    if (Utils::FileDialogs::FileIsOpened("openScene"))
    {
        // serialize the scene
        LOG_INFO("Serializing Scene");
        ResetScene(Utils::FileDialogs::m_SelectedFile);

        SceneSerializer serializer(m_Scene);
        serializer.Deserialize(Utils::FileDialogs::m_SelectedFile);
    }

    if (Utils::FileDialogs::FileIsSaved("saveScene"))
    {
        // serialize the scene
        LOG_INFO("Serializing Scene");
        m_Scene->SetSceneFilePath(Utils::FileDialogs::m_SavedFile);

        SceneSerializer serializer(m_Scene);
        serializer.Serialize(Utils::FileDialogs::m_SavedFile);
    }

    if (Utils::FileDialogs::FolderIsOpened("newProject"))
    {
        Project::New(Utils::FileDialogs::m_SelectedFolder);
    }
}

void AppLayer::UI_Toolbar()
{
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{0, 2});
    ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, ImVec2{0, 0});
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0, 0, 0, 0});
    auto &colors = ImGui::GetStyle().Colors;
    const auto &buttonHovered = colors[ImGuiCol_ButtonHovered];
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{buttonHovered.x, buttonHovered.y, buttonHovered.z, 0.5f});
    const auto &buttonActive = colors[ImGuiCol_ButtonActive];
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{buttonActive.x, buttonActive.y, buttonActive.z, 0.5f});

    ImGui::Begin("##toolbar", nullptr,
                 ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
    auto icon = m_SceneState == SceneState::Edit ? m_IconPlay : m_IconStop;
    auto size = ImGui::GetWindowHeight() - 4.0f;
    ImGui::SetCursorPosX((ImGui::GetWindowContentRegionMax().x * 0.5f) - (size * 0.5));
    if (ImGui::ImageButton((ImTextureID)icon, ImVec2{size, size}, ImVec2{0, 0}, ImVec2{1, 1}, 0))
    {
        if (m_SceneState == SceneState::Edit)
            OnScenePlay();
        else if (m_SceneState == SceneState::Play)
            OnSceneStop();
    }
    ImGui::PopStyleVar(2);
    ImGui::PopStyleColor(3);
    ImGui::End();
}
} // namespace Engine