#include "AppLayer.h"

#include <iostream>
#include <filesystem>

#include <imgui.h>
#include <ImGuizmo.h>
#include "Math/IMath.h"
#include "AssetManager.h"
#include "TextureImporter.h"
#include "Utils/FileDialogs.h"
#include "Prefab.h"

#include <IconsFontAwesome5.h>

namespace Engine
{
Texture2DRef stepForwardIcon, playIcon, pauseIcon, stopIcon;

WindowState windowState = InputManager::Get().GetWindowState();

AppLayer::AppLayer() : m_EditorCamera(-45.0f, 1.778f, 0.01f, 100.0f) {}

void AppLayer::OnAttach()
{
    // TODO: load last opened project from a savefile
    Project::Load("SandboxProject/SandboxProject.3dproj");

    m_Framebuffer = std::make_shared<Framebuffer>(true, glm::vec2{1280, 900});
    m_Framebuffer->SetTexture(std::make_shared<Texture2D>(ImageFormat::Depth), GL_DEPTH_ATTACHMENT);
    m_Framebuffer->SetTexture(std::make_shared<Texture2D>(ImageFormat::RGBA8), GL_COLOR_ATTACHMENT0);

    stepForwardIcon = TextureImporter::LoadTexture2D("Resources/Textures/StepForward.png");
    playIcon = TextureImporter::LoadTexture2D("Resources/Textures/Play.png");
    pauseIcon = TextureImporter::LoadTexture2D("Resources/Textures/Pause.png");
    stopIcon = TextureImporter::LoadTexture2D("Resources/Textures/Stop.png");

    m_EditorScene = std::make_shared<Scene>();
    m_ActiveScene = m_EditorScene;

    m_SceneHierarchyPanel.SetContext(m_ActiveScene);
    m_EnvironmentPanel.SetContext(m_ActiveScene);
    m_ContentBrowserPanel.SetContext(m_ActiveScene);
    // attach scene
    m_ActiveScene->OnAttach();

    LOG_INFO("AppLayer Attached");
}

void AppLayer::OnDetach() {}

void AppLayer::OnUpdate(float dt)
{
    m_IsControlPressed = false;

    // update
    m_EditorCamera.OnUpdate(dt);
    m_EditorCamera.SetViewportSize(m_ViewportSize.x, m_ViewportSize.y);
    m_ActiveScene->SetViewportSize(static_cast<int>(m_ViewportSize.x), static_cast<int>(m_ViewportSize.y));
    m_ActiveScene->SetFramebuffer(m_Framebuffer);

    m_ActiveScene->OnUpdate(dt);

    switch (m_SceneState)
    {
        case SceneState::Edit: m_ActiveScene->OnUpdateEditor(dt, m_EditorCamera); break;
        case SceneState::Play:
        {
            m_ActiveScene->OnUpdate(dt);
            m_ActiveScene->OnRuntimeUpdate(dt);
        }
        break;
        default: break;
    }

    auto [mx, my] = ImGui::GetMousePos();
    mx -= m_ViewportBounds[0].x;
    my -= m_ViewportBounds[0].y;
    const glm::vec2 viewportSize = m_ViewportBounds[1] - m_ViewportBounds[0];

    const int mouseX = static_cast<int>(mx);
    const int mouseY = static_cast<int>(my);

    if (mouseX >= 0 && mouseY >= 0 && mouseX < static_cast<int>(viewportSize.x) &&
        mouseY < static_cast<int>(viewportSize.y))
        m_ActiveScene->SetViewportMousePos(mouseX, mouseY);
    else
        m_ActiveScene->SetViewportMousePos(-1, -1);

    if (InputManager::Get().IsKeyPressed(InputKey::Escape)) OnSceneStop();
}

void AppLayer::OnFixedUpdate(float dt)
{
    // physic
    if (m_SceneState == SceneState::Play)
    {
        m_ActiveScene->OnFixedUpdate(dt);
    }
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
            if (ImGui::MenuItem("Open Project", "Ctrl+Shift+O")) OpenProject();
            ImGui::Separator();
            if (ImGui::MenuItem("Save...", "Ctrl+S")) SaveScene();
            if (ImGui::MenuItem("Save As...", "Ctrl+Shift+S")) SaveSceneAs();
            ImGui::Separator();
            if (ImGui::MenuItem("Exit", "")) Application::Close();

            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Project"))
        {
            if (ImGui::MenuItem("Input Map")) m_InputMapPanel.SetOpen(true);
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu(".NET"))
        {
            if (ImGui::MenuItem("Generate Solution")) m_ActiveScene->GenerateNETSolution();
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }

    m_SceneHierarchyPanel.OnImGuiRender();
    m_EnvironmentPanel.OnImGuiRender();
    m_MaterialEditorPanel.OnImGuiRender();
    m_ContentBrowserPanel.OnImGuiRender();
    m_InputMapPanel.OnImGuiRender();

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{0, 0});
    ImGui::Begin("Viewport");
    auto viewportOffset = ImGui::GetCursorPos();

    m_ViewportFocused = ImGui::IsWindowFocused();
    m_ViewportHovered = ImGui::IsWindowHovered();
    auto viewportPanelSize = ImGui::GetContentRegionAvail();
    if (m_ViewportSize != *((glm::vec2 *)&viewportPanelSize))
    {
        m_Framebuffer->QueueResize(glm::vec2(viewportPanelSize.x, viewportPanelSize.y));
        m_ViewportSize = {viewportPanelSize.x, viewportPanelSize.y};
    }
    ImGui::Image((void *)(intptr_t)m_Framebuffer->GetTexture(GL_COLOR_ATTACHMENT0)->GetRendererID(),
                 ImVec2{m_ViewportSize.x, m_ViewportSize.y});

    if (ImGui::BeginDragDropTarget())
    {
        if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
        {
            const char *path = (const char *)payload->Data;
            auto assetType = Utils::GetAssetTypeFromExtension(path);

            switch (assetType)
            {
                case AssetType::Scene:
                {
                    ResetScene("");
                    m_EditorScene = AssetManager::GetAsset<Scene>(path);
                    m_SceneHierarchyPanel.SetContext(m_EditorScene);
                    m_EnvironmentPanel.SetContext(m_EditorScene);

                    m_ActiveScene = m_EditorScene;
                }
                break;
                case AssetType::Mesh:
                {
                    auto ent = m_ActiveScene->CreateEntity("Mesh");
                    auto &mesh = ent.AddComponent<MeshComponent>();
                    mesh.Handle = AssetManager::GetAssetHandleFromPath(path);
                }
                break;
                case AssetType::Material:
                {
                    if (m_ActiveScene->GetHoveredEntity() == (entt::entity)-1) break;

                    auto handle = AssetManager::GetAssetHandleFromPath(path);
                    // get current hovered entity, add material
                    Entity ent = {m_ActiveScene->GetHoveredEntity(), m_ActiveScene.get()};
                    auto &mesh = ent.GetComponent<MeshComponent>();
                    mesh.MaterialHandle = handle;
                }
                break;
                case AssetType::Prefab:
                {
                    auto prefab = AssetManager::GetAsset<Prefab>(path);
                    prefab->AttachToScene(m_ActiveScene);
                }
                break;
                case AssetType::None: LOG_CORE_ERROR("Unknown Asset type!"); break;
                default: break;
            }
        }
        ImGui::EndDragDropTarget();
    }

    // controls
    ImGui::SetItemAllowOverlap();
    ImGui::SetCursorPos({10, 40});

    // ImGuiIO &io = ImGui::GetIO();
    float oldSize = ImGui::GetFont()->Scale;
    float buttonSize = 42;
    ImVec4 activeColor = ImVec4(0.9255f, 0.6196f, 0.1412f, 1.0f);

    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{5, 0});
    ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2{0.5, 0.5});
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 5.0f);

    // color to show it is selected
    DrawControls(ICON_FA_MOUSE_POINTER, "Select", m_GizmoType == -1, [&]() { m_GizmoType = -1; });
    ImGui::SameLine();
    DrawControls(ICON_FA_ARROWS_ALT, "Move", m_GizmoType == ImGuizmo::OPERATION::TRANSLATE,
                 [&]() { m_GizmoType = ImGuizmo::OPERATION::TRANSLATE; });
    ImGui::SameLine();
    DrawControls(ICON_FA_SYNC_ALT, "Rotate", m_GizmoType == ImGuizmo::OPERATION::ROTATE,
                 [&]() { m_GizmoType = ImGuizmo::OPERATION::ROTATE; });
    ImGui::SameLine();
    DrawControls(ICON_FA_EXPAND_ARROWS_ALT, "Scale", m_GizmoType == ImGuizmo::OPERATION::SCALE,
                 [&]() { m_GizmoType = ImGuizmo::OPERATION::SCALE; });
    ImGui::SameLine();

    // draw to far right
    ImGui::SetCursorPosX(ImGui::GetWindowWidth() - (buttonSize * 2) - 10);
    DrawControls(ICON_FA_BUG, "Show/Hide Physics Debug", m_ActiveScene->IsDebugDrawEnabled(),
                 [&]() { m_ActiveScene->SetDebugDraw(!m_ActiveScene->IsDebugDrawEnabled()); });
    ImGui::SameLine();
    // show/hide grid
    DrawControls(ICON_FA_BORDER_ALL, "Show/Hide Grid", m_ActiveScene->IsGridEnabled(),
                 [&]() { m_ActiveScene->SetGridEnabled(!m_ActiveScene->IsGridEnabled()); });

    ImGui::PopStyleVar(3);

    auto windowSize = ImGui::GetWindowSize();
    auto miniBound = ImGui::GetWindowPos();
    miniBound.x += viewportOffset.x;
    miniBound.y += viewportOffset.y;

    auto maxBound = ImVec2(miniBound.x + windowSize.x, miniBound.y + windowSize.y);
    m_ViewportBounds[0] = {miniBound.x, miniBound.y};
    m_ViewportBounds[1] = {maxBound.x, maxBound.y};

    // Gizmos
    auto selectedEntity = m_SceneHierarchyPanel.GetSelectedEntity();

    glm::mat4 cameraView = m_EditorCamera.GetViewMatrix();
    glm::mat4 projection = m_EditorCamera.GetProjectionMatrix();

    cameraView[0][1] = -cameraView[0][1];
    cameraView[1][1] = -cameraView[1][1];
    cameraView[2][1] = -cameraView[2][1];

    ImGuizmo::SetOrthographic(false);
    ImGuizmo::SetDrawlist();
    ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, m_ViewportSize.x, m_ViewportSize.y);

    if (selectedEntity && m_GizmoType != -1 && m_SceneState == SceneState::Edit)
    {
        // Entity Transform
        if (selectedEntity.HasComponent<TransformComponent>())
        {
            auto &tc = selectedEntity.GetComponent<TransformComponent>();
            auto parentComponent = selectedEntity.GetComponent<ParentComponent>();

            glm::mat4 transform = tc.GetTransform();

            // Snapping
            bool snap = InputManager::Get().IsKeyPressed(InputKey::LeftControl);
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

                if (parentComponent.HasParent)
                {
                    // get local transform from global transform
                    auto parentEntity = m_ActiveScene->GetEntityByUUID(parentComponent.Parent);
                    const auto &parentTransform = parentEntity.GetComponent<TransformComponent>();
                    glm::mat4 parentTransformMat = parentTransform.GetTransform();
                    glm::mat4 localTransform = glm::inverse(parentTransformMat) * tc.GetTransform();

                    tc.SetLocalTransform(localTransform);
                }
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
    auto Input = InputManager::Get();

    const auto ctrl = InputManager::Get().IsSpecialKeyPressed(SpecialKey::Control);
    const auto shift = InputManager::Get().IsSpecialKeyPressed(SpecialKey::Shift);

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
        case InputKey::D:
            if (ctrl) DuplicateEntity();
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
    if (m_ViewportHovered) m_EditorCamera.OnMouseScrolled(xOffset, yOffset);
}

void AppLayer::OnMouseButtonPressed(MouseButton button)
{
    if (!m_IsControlPressed)
    {
        auto Input = InputManager::Get();
        // Mouse picking
        if (button == MouseButton::Left && !ImGuizmo::IsOver() && !Input.IsKeyPressed(InputKey::LeftAlt))
        {
            if (m_ViewportHovered)
            {
                m_SceneHierarchyPanel.SetSelectedEntity({m_ActiveScene->GetHoveredEntity(), m_ActiveScene.get()});
            }
        }
    }
}

void AppLayer::NewProject()
{
    auto path = FileDialogs::SaveFile("3D Engine Project (*.3dproj)\0*.3dproj\0");
    if (!path.empty())
    {
        Project::New(path);
        Project::Load(path);
    }
}

void AppLayer::OpenProject()
{
    auto path = FileDialogs::OpenFile("3D Engine Project (*.3dproj)\0*.3dproj\0");
    if (!path.empty())
    {
        Project::Load(path);
    }
}

void AppLayer::NewScene()
{
    // TODO: create new scene file
    ResetScene("");
    m_ActiveScene = std::make_unique<Scene>();
    m_SceneHierarchyPanel.SetContext(m_ActiveScene);
    m_EnvironmentPanel.SetContext(m_ActiveScene);
}

void AppLayer::OpenScene()
{
    // Fix later
    auto path = FileDialogs::OpenFile("3D Engine Scene (*.scene)\0*.scene\0");
    if (!path.empty())
    {
        ResetScene("");

        auto handle = AssetManager::ImportAsset(path);
        m_EditorScene = AssetManager::GetAsset<Scene>(handle);
        m_SceneHierarchyPanel.SetContext(m_EditorScene);
        m_EnvironmentPanel.SetContext(m_EditorScene);

        m_ActiveScene = m_EditorScene;
    }
}

void AppLayer::SaveSceneAs()
{
    auto path = FileDialogs::SaveFile("3D Engine Scene (*.scene)\0*.scene\0");
    if (!path.empty())
    {
        m_ActiveScene->SetSceneFilePath(path);
        SceneSerializer serializer(m_ActiveScene);
        serializer.Serialize(path);
    }
}

void AppLayer::SaveScene()
{
    if (m_ActiveScene->GetSceneFilePath().empty())
        SaveSceneAs();
    else
    {
        SceneSerializer serializer(m_ActiveScene);
        serializer.Serialize(m_ActiveScene->GetSceneFilePath());
    }
}

void AppLayer::ResetScene(const std::string &path) { m_EditorCamera = EditorCamera(-45.0f, 1.778f, 0.1f, 100.0f); }

void AppLayer::DuplicateEntity()
{
    if (m_SceneState == SceneState::Edit)
    {
        Entity selectedEntity = m_SceneHierarchyPanel.GetSelectedEntity();
        if (selectedEntity) m_ActiveScene->DuplicateEntity(selectedEntity);
    }
}

void AppLayer::OnScenePlay()
{
    m_SceneState = SceneState::Play;
    m_ActiveScene = Scene::Copy(m_EditorScene);

    m_ActiveScene->SetPlaying(true);
    m_ActiveScene->OnRuntimeStart();

    m_SceneHierarchyPanel.SetContext(m_ActiveScene);
    m_EnvironmentPanel.SetContext(m_ActiveScene);
}

void AppLayer::OnSceneStop()
{
    // TODO: fix screen jittering when stopping
    m_SceneState = SceneState::Edit;
    m_ActiveScene = m_EditorScene;

    m_ActiveScene->SetPlaying(false);
    m_ActiveScene->OnRuntimeStop();

    m_SceneHierarchyPanel.SetContext(m_ActiveScene);
    m_EnvironmentPanel.SetContext(m_ActiveScene);
}

void AppLayer::UI_Toolbar()
{
    ImGui::Begin("##toolbar", nullptr,
                 ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
    auto icon = m_SceneState == SceneState::Edit ? playIcon : stopIcon;
    auto size = ImGui::GetWindowHeight() - 6.0f;
    ImGui::SetCursorPosX((ImGui::GetWindowContentRegionMax().x * 0.5f) - (size * 0.5));
    ImGui::SetCursorPosY((ImGui::GetWindowContentRegionMax().y * 0.5f) - (size * 0.5));
    if (ImGui::ImageButton((void *)(intptr_t)icon->GetRendererID(), ImVec2{size, size}))
    {
        if (m_SceneState == SceneState::Edit)
            OnScenePlay();
        else if (m_SceneState == SceneState::Play)
            OnSceneStop();
    }
    ImGui::SameLine(0, 10.0f);
    // disabled pause button
    if (!m_ActiveScene->IsPlaying())
    {
        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
        ImGui::ImageButton((void *)(intptr_t)pauseIcon->GetRendererID(), ImVec2{size, size});
        ImGui::SameLine(0, 10.0f);
        ImGui::ImageButton((void *)(intptr_t)stepForwardIcon->GetRendererID(), ImVec2{size, size});

        ImGui::PopStyleVar();
    }
    else
    {
        if (!m_ActiveScene->IsPaused() &&
            ImGui::ImageButton((void *)(intptr_t)pauseIcon->GetRendererID(), ImVec2{size, size}))
        {
            m_ActiveScene->SetPaused(true);
            m_ActiveScene->OnRuntimeStop();
        }
        else if (m_ActiveScene->IsPaused() &&
                 ImGui::ImageButton((void *)(intptr_t)playIcon->GetRendererID(), ImVec2{size, size}))
        {
            m_ActiveScene->SetPaused(false);
            m_ActiveScene->OnRuntimeStart();
        }
        else
        {
            ImGui::SameLine(0, 10.0f);
            if (ImGui::ImageButton((void *)(intptr_t)stepForwardIcon->GetRendererID(), ImVec2{size, size}))
                m_ActiveScene->StepRuntimeFrame(10);
        }
    }
    ImGui::End();
}

void AppLayer::DrawControls(const char *icon, const char *tooltip, bool isActive, std::function<void()> action)
{
    float buttonSize = 42;
    constexpr auto activeColor = ImVec4(0.0f, 0.447f, 0.776f, 1.0f);

    // change color of selected button
    if (isActive)
    {
        ImGui::PushStyleColor(ImGuiCol_Button, activeColor);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, activeColor);
    }
    else
    {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.2f, 0.2f, 0.2f, 1.0f});
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{0.3f, 0.3f, 0.3f, 1.0f});
    }

    ImGui::GetFont()->Scale *= 1.2;
    ImGui::PushFont(ImGui::GetFont());
    if (ImGui::Button(icon, {buttonSize, buttonSize})) action();
    ImGui::PopFont();
    ImGui::GetFont()->Scale /= 1.2;

    if (ImGui::IsItemHovered())
    {
        m_IsControlPressed = true;
    }
    // tooltip
    if (ImGui::IsItemHovered())
    {
        ImGui::BeginTooltip();
        ImGui::GetFont()->Scale *= 0.9;
        ImGui::PushFont(ImGui::GetFont());
        ImGui::TextUnformatted(tooltip);
        ImGui::PopFont();
        ImGui::GetFont()->Scale /= 0.9;
        ImGui::EndTooltip();
    }

    ImGui::PopStyleColor(2);
}
} // namespace Engine
