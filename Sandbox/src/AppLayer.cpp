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
#include "PrefabSerializer.h"
#include "Mesh.h"
#include "SceneManager.h"

#include <IconsFontAwesome5.h>

namespace Engine
{
Texture2DRef stepForwardIcon, playIcon, pauseIcon, stopIcon;

WindowState windowState = InputManager::Get().GetWindowState();

AppLayer::AppLayer() {}

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

	m_ContentBrowserPanel = std::make_unique<ContentBrowserPanel>();

    SetPanelsContext();
    // attach scene
	SceneManager::Get().GetActiveScene()->OnAttach();

    LOG_INFO("AppLayer Attached");
}

void AppLayer::OnDetach() {}

void AppLayer::OnUpdate(float dt)
{
    m_IsControlPressed = false;

    // update
    SceneManager::Get().GetActiveScene()->GetEditorCamera()->OnUpdate(dt);
    SceneManager::Get().GetActiveScene()->GetEditorCamera()->SetViewportSize(m_ViewportSize.x, m_ViewportSize.y);
    SceneManager::Get().GetActiveScene()->SetViewportSize(static_cast<int>(m_ViewportSize.x), static_cast<int>(m_ViewportSize.y));
    SceneManager::Get().GetActiveScene()->SetFramebuffer(m_Framebuffer);

    SceneManager::Get().GetActiveScene()->OnUpdate(dt);

    switch (SceneManager::Get().GetSceneState())
    {
        case SceneState::Edit:
			SceneManager::Get().GetActiveScene()->OnUpdateEditor(dt, *SceneManager::Get().GetActiveScene()->GetEditorCamera());
            break;
        case SceneState::Play:
			SceneManager::Get().GetActiveScene()->OnRuntimeUpdate(dt); 
			break;
        default: break;
    }

    auto [mx, my] = ImGui::GetMousePos();
    mx -= m_ViewportBounds[0].x;
    my -= m_ViewportBounds[0].y;
    const glm::vec2 viewportSize = m_ViewportBounds[1] - m_ViewportBounds[0];

    const int mouseX = static_cast<int>(mx);
    const int mouseY = static_cast<int>(my);

    if (mouseX >= 0 && mouseY >= 0 && mouseX < static_cast<int>(viewportSize.x) && mouseY < static_cast<int>(viewportSize.y))
        SceneManager::Get().GetActiveScene()->SetViewportMousePos(mouseX, mouseY);
    else SceneManager::Get().GetActiveScene()->SetViewportMousePos(-1, -1);

    if (InputManager::Get().IsKeyPressed(InputKey::Escape)) SceneManager::Get().OnSceneStop();
}

void AppLayer::OnFixedUpdate(float dt)
{
    // physics
    if (SceneManager::Get().GetSceneState() == SceneState::Play) SceneManager::Get().GetActiveScene()->OnFixedUpdate(dt);
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
        window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
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
                SceneManager::Get().NewScene();
				SetPanelsContext();
            }
            if (ImGui::MenuItem("Open...", "Ctrl+O"))
            {
                SceneManager::Get().OpenScene();
				SetPanelsContext();
            }
            ImGui::Separator();
            if (ImGui::MenuItem("New Project", "Ctrl+Shift+N")) NewProject();
            if (ImGui::MenuItem("Open Project", "Ctrl+Shift+O")) OpenProject();
            ImGui::Separator();
            if (ImGui::MenuItem("Save...", "Ctrl+S")) SceneManager::Get().SaveScene();
            if (ImGui::MenuItem("Save As...", "Ctrl+Shift+S")) SceneManager::Get().SaveSceneAs();
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
            if (ImGui::MenuItem("Generate Solution")) SceneManager::Get().GetActiveScene()->GenerateNETSolution();
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }

    m_SceneHierarchyPanel.OnImGuiRender();
    m_EnvironmentPanel.OnImGuiRender();
    m_MaterialEditorPanel.OnImGuiRender();
    m_ContentBrowserPanel->OnImGuiRender();
    m_InputMapPanel.OnImGuiRender();
	m_ConsolePanel.OnImGuiRender();

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
    ImGui::Image((void *)(intptr_t)m_Framebuffer->GetTexture(GL_COLOR_ATTACHMENT0)->GetRendererID(), ImVec2{m_ViewportSize.x, m_ViewportSize.y});

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
                    auto editorScene = AssetManager::GetAsset<Scene>(path);
                    SceneManager::Get().SetEditorScene(editorScene);
                    SceneManager::Get().SetActiveScene(editorScene);

                    m_SceneHierarchyPanel.SetContext(SceneManager::Get().GetActiveScene());
                    m_EnvironmentPanel.SetContext(SceneManager::Get().GetActiveScene());
                }
                break;
                case AssetType::Mesh:
                {
                    const auto handle = AssetManager::GetAssetHandleFromPath(path);
					const auto &asset = AssetManager::GetAsset<Mesh>(path);
					if (!asset->HasAnimations())
					{
						// make prefab from asset
                        auto prefabAsset = AssetManager::GetAsset<Prefab>(path);
                        PrefabSerializer serializer(SceneManager::Get().GetActiveScene());
                        serializer.Deserialize(asset->StaticMeshes, handle, prefabAsset->Handle);
					}
					else
					{
						auto ent = SceneManager::Get().GetActiveScene()->CreateEntity("Mesh");
						LOG_CORE_INFO("Loading skinned mesh");
						auto &mesh = ent.AddComponent<SkinnedMeshComponent>();
						mesh.Handle = handle;
						ent.GetComponent<TagComponent>().Tag = AssetManager::GetAssetName(handle);
						auto &animationController = ent.AddComponent<AnimationControllerComponent>();
						for (int i = 0; i < asset->SkinnedMeshData.NumAnimations; i++)
						{
							auto animation = new Animation();
							animation->LoadAnimation(Project::GetAssetDirectory() / path, &asset->SkinnedMeshData, i);
							animationController.AddAnimation(animation);
						}
						animationController.Animator = new Animator(animationController.Animations[0]);
						SceneManager::Get().GetActiveScene()->AddToRootEntity(ent);
					}
                }
                break;
                case AssetType::Material:
                {
                    if (SceneManager::Get().GetActiveScene()->GetHoveredEntity() == (entt::entity)-1) break;

                    auto handle = AssetManager::GetAssetHandleFromPath(path);
                    // get current hovered entity, add material
                    Entity ent = {SceneManager::Get().GetActiveScene()->GetHoveredEntity(), SceneManager::Get().GetActiveScene().get()};
                    auto &mesh = ent.GetComponent<StaticMeshComponent>();
                    mesh.MaterialHandle = handle;
                }
                break;
                case AssetType::Prefab:
                {
					auto asset = AssetManager::GetAsset<Prefab>(path);
                    PrefabSerializer serializer(SceneManager::Get().GetActiveScene());
					auto entity = serializer.Deserialize(Project::GetAssetDirectory() / AssetManager::GetRegistry()[asset->Handle].FilePath);
                    entity.GetComponent<TagComponent>().Tag = AssetManager::GetAssetName(asset->Handle);
					auto &tc = entity.GetComponent<TransformComponent>();
                    tc.Translation = {0, 0, 0};
                    tc.Rotation = {0, 0, 0, 0};
                }
                break;
                case AssetType::SkyLight:
				{
                    auto environment = SceneManager::Get().GetActiveScene()->GetEnvironment();
                    environment->CurrentSkyType = SkyType::SkyboxHDR;
                    environment->SkyboxHDR = AssetManager::GetAsset<SkyLight>(path);
				}
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
    //ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 5.0f);

    // color to show it is selected
    DrawControls(ICON_FA_MOUSE_POINTER, "Select", m_GizmoType == -1, [&]() { m_GizmoType = -1; });
    ImGui::SameLine();
    DrawControls(ICON_FA_ARROWS_ALT, "Move", m_GizmoType == ImGuizmo::OPERATION::TRANSLATE, [&]() { m_GizmoType = ImGuizmo::OPERATION::TRANSLATE; });
    ImGui::SameLine();
    DrawControls(ICON_FA_SYNC_ALT, "Rotate", m_GizmoType == ImGuizmo::OPERATION::ROTATE, [&]() { m_GizmoType = ImGuizmo::OPERATION::ROTATE; });
    ImGui::SameLine();
    DrawControls(ICON_FA_EXPAND_ARROWS_ALT, "Scale", m_GizmoType == ImGuizmo::OPERATION::SCALE, [&]() { m_GizmoType = ImGuizmo::OPERATION::SCALE; });
    ImGui::SameLine();

    // draw to far right
    ImGui::SetCursorPosX(ImGui::GetWindowWidth() - (buttonSize * 2) - 10);
    DrawControls(ICON_FA_BUG, "Show/Hide Physics Debug", SceneManager::Get().GetActiveScene()->IsDebugDrawEnabled(), 
		[&]() { SceneManager::Get().GetActiveScene()->SetDebugDraw(!SceneManager::Get().GetActiveScene()->IsDebugDrawEnabled()); });
    ImGui::SameLine();
    // show/hide grid
    DrawControls(ICON_FA_BORDER_ALL, "Show/Hide Grid", SceneManager::Get().GetActiveScene()->IsGridEnabled(), 
		[&]() { SceneManager::Get().GetActiveScene()->SetGridEnabled(!SceneManager::Get().GetActiveScene()->IsGridEnabled()); });

    ImGui::PopStyleVar(2);

    auto windowSize = ImGui::GetWindowSize();
    auto miniBound = ImGui::GetWindowPos();
    miniBound.x += viewportOffset.x;
    miniBound.y += viewportOffset.y;

    auto maxBound = ImVec2(miniBound.x + windowSize.x, miniBound.y + windowSize.y);
    m_ViewportBounds[0] = {miniBound.x, miniBound.y};
    m_ViewportBounds[1] = {maxBound.x, maxBound.y};

    // Gizmos
    auto selectedEntity = m_SceneHierarchyPanel.GetSelectedEntity();

    glm::mat4 cameraView = SceneManager::Get().GetActiveScene()->GetEditorCamera()->GetViewMatrix();
    glm::mat4 projection = SceneManager::Get().GetActiveScene()->GetEditorCamera()->GetProjectionMatrix();

    cameraView[0][1] = -cameraView[0][1];
    cameraView[1][1] = -cameraView[1][1];
    cameraView[2][1] = -cameraView[2][1];
	cameraView[3][1] = -cameraView[3][1];

    ImGuizmo::SetOrthographic(false);
    ImGuizmo::SetDrawlist();
    ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, m_ViewportSize.x, m_ViewportSize.y);

    if (selectedEntity && m_GizmoType != -1 && SceneManager::Get().GetSceneState() == SceneState::Edit)
    {
        // Entity Transform
        if (selectedEntity.HasComponent<TransformComponent>())
        {
            auto &tc = selectedEntity.GetComponent<TransformComponent>();
            auto parentComponent = selectedEntity.GetComponent<ParentComponent>();

            glm::mat4 transform = tc.GetGlobalTransform();

            // Snapping
            bool snap = InputManager::Get().IsKeyPressed(InputKey::LeftControl);
            float snapValue = m_GizmoType == ImGuizmo::OPERATION::ROTATE ? 45.0f : 0.5f;
            float snapValues[3] = {snapValue, snapValue, snapValue};

            ImGuizmo::Manipulate(glm::value_ptr(cameraView), glm::value_ptr(projection),
                                 (ImGuizmo::OPERATION)m_GizmoType, ImGuizmo::MODE::LOCAL, glm::value_ptr(transform),
                                 nullptr, snap ? snapValues : nullptr);

            if (ImGuizmo::IsUsing())
            {
                // Since imguizmo returns a transform in global space and we want the local transform,
                // we need to multiply by the inverse of the parent's global transform in order to revert
                // the changes from the parent transform.
                glm::mat4 localTransform = glm::mat4(transform);
                ParentComponent &parent = selectedEntity.GetComponent<ParentComponent>();
                if (parent.HasParent)
                {
                    const auto &parentTransformComponent = SceneManager::Get().GetActiveScene()->GetEntityByUUID(parent.Parent).GetComponent<TransformComponent>();
                    const glm::mat4 &parentTransform = parentTransformComponent.GetGlobalTransform();
                    localTransform = glm::inverse(parentTransform) * localTransform;
                }

                // Decompose local transform
                float decomposedPosition[3];
                float decomposedEuler[3];
                float decomposedScale[3];
                ImGuizmo::DecomposeMatrixToComponents(glm::value_ptr(localTransform), decomposedPosition, decomposedEuler, decomposedScale);

                const auto &localPosition = glm::vec3(decomposedPosition[0], decomposedPosition[1], decomposedPosition[2]);
                const auto &localScale = glm::vec3(decomposedScale[0], decomposedScale[1], decomposedScale[2]);

                localTransform[0] /= localScale.x;
                localTransform[1] /= localScale.y;
                localTransform[2] /= localScale.z;
                const auto &rotationMatrix = glm::mat3(localTransform);
                const glm::quat &localRotation = glm::normalize(glm::quat(rotationMatrix));

                const glm::mat4 &rotationMatrix4 = glm::mat4_cast(localRotation);
                const glm::mat4 &scaleMatrix = glm::scale(glm::mat4(1.0f), localScale);
                const glm::mat4 &translationMatrix = glm::translate(glm::mat4(1.0f), localPosition);
                const glm::mat4 &newLocalTransform = translationMatrix * rotationMatrix4 * scaleMatrix;

                tc.Translation = localPosition;

                if (m_GizmoType != ImGuizmo::SCALE) tc.Rotation = localRotation;

                tc.Scale = localScale;
                tc.LocalTransform = newLocalTransform;
                tc.Dirty = true;
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

        case InputKey::N: if (ctrl) SceneManager::Get().NewScene(); break;
        case InputKey::O: if (ctrl) SceneManager::Get().OpenScene(); break;
        case InputKey::D: if (ctrl) DuplicateEntity(); break;
        case InputKey::S:
        {
            if (ctrl && shift) SceneManager::Get().SaveSceneAs();
            else if (ctrl) SceneManager::Get().SaveScene();
			break;
        }
        default: break;
    }
}

void AppLayer::OnMouseScrolled(double xOffset, double yOffset)
{
    if (m_ViewportHovered) SceneManager::Get().GetActiveScene()->GetEditorCamera()->OnMouseScrolled(xOffset, yOffset);
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
                Entity e = {SceneManager::Get().GetActiveScene()->GetHoveredEntity(), SceneManager::Get().GetActiveScene().get()};
                m_SceneHierarchyPanel.SetSelectedEntity(e);
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

void AppLayer::ResetScene(const std::string& path) 
{ 
	SetPanelsContext();
}

void AppLayer::DuplicateEntity()
{
    if (SceneManager::Get().GetSceneState() == SceneState::Edit)
    {
        if (SceneManager::Get().GetActiveScene()->GetSceneType() == SceneType::Scene3D)
        {
            auto selectedEntity = m_SceneHierarchyPanel.GetSelectedEntity();
            auto parent = SceneManager::Get().GetActiveScene()->GetEntityByUUID(selectedEntity.GetComponent<ParentComponent>().Parent);
            if (selectedEntity) SceneManager::Get().GetActiveScene()->DuplicateEntityRecursive(selectedEntity, parent);
        } 
		else if (SceneManager::Get().GetActiveScene()->GetSceneType() == SceneType::Prefab3D)
		{
			// spawn new prefab
			auto selectedEntity = m_SceneHierarchyPanel.GetSelectedEntity();
            auto asset = AssetManager::GetAsset<Prefab>(selectedEntity.GetComponent<PrefabInstanceComponent>().PrefabID);
            PrefabSerializer serializer(SceneManager::Get().GetActiveScene());
            auto entity = serializer.Deserialize(Project::GetAssetDirectory() / AssetManager::GetRegistry()[asset->Handle].FilePath);
            entity.GetComponent<TagComponent>().Tag = AssetManager::GetAssetName(asset->Handle);
		}
    }
}

void AppLayer::UI_Toolbar()
{
    ImGui::Begin("##toolbar", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
    auto icon = SceneManager::Get().GetSceneState() == SceneState::Edit ? playIcon : stopIcon;
    auto size = ImGui::GetWindowHeight() - 6.0f;
    ImGui::SetCursorPosX((ImGui::GetWindowContentRegionMax().x * 0.5f) - (size * 0.5));
    ImGui::SetCursorPosY((ImGui::GetWindowContentRegionMax().y * 0.5f) - (size * 0.5));
    if (ImGui::ImageButton((void *)(intptr_t)icon->GetRendererID(), ImVec2{size, size}))
    {
        if (SceneManager::Get().GetSceneState() == SceneState::Edit) SceneManager::Get().OnScenePlay();
        else if (SceneManager::Get().GetSceneState() == SceneState::Play) SceneManager::Get().OnSceneStop();
    }
    ImGui::SameLine(0, 10.0f);
    // disabled pause button
    if (!SceneManager::Get().GetActiveScene()->IsPlaying())
    {
        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
        ImGui::ImageButton((void *)(intptr_t)pauseIcon->GetRendererID(), ImVec2{size, size});
        ImGui::SameLine(0, 10.0f);
        ImGui::ImageButton((void *)(intptr_t)stepForwardIcon->GetRendererID(), ImVec2{size, size});

        ImGui::PopStyleVar();
    }
    else
    {
        if (!SceneManager::Get().GetActiveScene()->IsPaused() && ImGui::ImageButton((void *)(intptr_t)pauseIcon->GetRendererID(), ImVec2{size, size}))
        {
            SceneManager::Get().GetActiveScene()->SetPaused(true);
            SceneManager::Get().GetActiveScene()->OnRuntimeStop();
        }
        else if (SceneManager::Get().GetActiveScene()->IsPaused() && ImGui::ImageButton((void *)(intptr_t)playIcon->GetRendererID(), ImVec2{size, size}))
        {
            SceneManager::Get().GetActiveScene()->SetPaused(false);
            SceneManager::Get().GetActiveScene()->OnRuntimeStart();
        }
        else
        {
            ImGui::SameLine(0, 10.0f);
            if (ImGui::ImageButton((void *)(intptr_t)stepForwardIcon->GetRendererID(), ImVec2{size, size})) 
				SceneManager::Get().GetActiveScene()->StepRuntimeFrame(10);
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

    if (ImGui::IsItemHovered()) m_IsControlPressed = true;
    
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

void AppLayer::SetPanelsContext() 
{
    m_SceneHierarchyPanel.SetContext(SceneManager::Get().GetActiveScene());
    m_EnvironmentPanel.SetContext(SceneManager::Get().GetActiveScene());
	m_ContentBrowserPanel->SetContext(SceneManager::Get().GetActiveScene());
}
} // namespace Engine
