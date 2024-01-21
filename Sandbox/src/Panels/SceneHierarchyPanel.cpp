#include "SceneHierarchyPanel.h"

#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/glm.hpp>

#include "Light.h"
#include "MaterialEditorPanel.h"
#include "ImGuiHelpers.h"
#include "PhysicsComponents.h"
#include "MeshImporter.h"
#include "NetScript.h"

#include <IconsFontAwesome5.h>

#define ADD_COMPONENT_MENU(componentType, componentName)                                                               \
    if (!m_SelectionContext.HasComponent<componentType>())                                                             \
    {                                                                                                                  \
        if (ImGui::MenuItem(componentName))                                                                            \
        {                                                                                                              \
            m_SelectionContext.AddComponent<componentType>();                                                          \
            ImGui::CloseCurrentPopup();                                                                                \
        }                                                                                                              \
    }

#define REMOVABLE_COMPONENT                                                                                            \
    if (ImGui::BeginPopupContextItem())                                                                                \
    {                                                                                                                  \
        if (ImGui::MenuItem("Remove")) removeComponent = true;                                                         \
        ImGui::EndPopup();                                                                                             \
    }

namespace Engine
{
SceneHierarchyPanel::SceneHierarchyPanel(const std::shared_ptr<Scene> &context) { SetContext(context); }

void SceneHierarchyPanel::DrawEntityNode(Entity entity)
{
    const auto &tag = entity.GetComponent<TagComponent>().Tag;
    auto &parentComponent = entity.GetComponent<ParentComponent>();

    auto flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_FramePadding |
                 ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanFullWidth |
                 ImGuiTreeNodeFlags_DefaultOpen;

    if (m_SelectionContext == entity) flags |= ImGuiTreeNodeFlags_Selected;
    if (parentComponent.Children.size() <= 0) flags |= ImGuiTreeNodeFlags_Leaf;

    std::string icon;
    if (entity.HasComponent<PointLightComponent>())
        icon = ICON_FA_LIGHTBULB;
    else if (entity.HasComponent<SpotLightComponent>())
        icon = ICON_FA_LIGHTBULB;
    else if (entity.HasComponent<DirectionalLightComponent>())
        icon = ICON_FA_SUN;
    else if (entity.HasComponent<CameraComponent>())
        icon = ICON_FA_VIDEO;
    else
        icon = ICON_FA_CUBE;
    icon.append("  ");

    ImVec4 textColor = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
    if (entity.GetComponent<TagComponent>().IsPrefab)
    {
        textColor = ImVec4(0.0f, 1.0f, 0.0f, 1.0f);
    }

    ImGui::PushStyleColor(ImGuiCol_Text, textColor);
    bool open = ImGui::TreeNodeEx((void *)(uint64_t)(uint32_t)entity, flags, "%s", (icon + tag).c_str());
    ImGui::PopStyleColor();
    // select when clicked
    if (ImGui::IsItemClicked()) m_SelectionContext = entity;

    if (ImGui::BeginDragDropSource())
    {
        ImGui::SetDragDropPayload("ENTITY", (void *)&entity, sizeof(Entity));
        ImGui::Text(ICON_FA_CUBE " %s", entity.GetComponent<TagComponent>().Tag.c_str());
        ImGui::EndDragDropSource();
    }

    if (ImGui::BeginDragDropTarget())
    {
        if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("ENTITY"))
        {
            Entity payloadEntity = *(const Entity *)payload->Data;
            auto &pc = payloadEntity.GetComponent<ParentComponent>();
            auto payloadId = payloadEntity.GetComponent<IDComponent>().ID;
        }
        ImGui::EndDragDropTarget();
    }

    bool entityDeleted = false;
    if (ImGui::BeginPopupContextItem())
    {
        if (ImGui::BeginMenu(ICON_FA_PLUS "  Add Child"))
        {
            auto childEntity = CreateEntityPopup();
            if (childEntity)
            {
                entity.AddChild(childEntity);
                m_SelectionContext = childEntity;
            }

            ImGui::EndPopup();
        }
        ImGui::Separator();
        if (ImGui::MenuItem("Duplicate Entity"))
        {
            auto newEntity = m_Context->DuplicateEntityRecursive(entity);
            m_SelectionContext = newEntity;
        }

        if (parentComponent.HasParent && ImGui::MenuItem("Move to root"))
        {
            if (parentComponent.HasParent)
            {
                auto p = m_Context->GetEntityByUUID(parentComponent.Parent);
                p.GetComponent<ParentComponent>().RemoveChild(entity.GetComponent<IDComponent>().ID);
                parentComponent.HasParent = false;
            }
        }
        if (ImGui::MenuItem("Delete Entity"))
        {
            m_SelectionContext = entity;
            entityDeleted = true;
        }
        ImGui::EndPopup();
    }

    if (entityDeleted)
    {
        if (entity.HasComponent<DirectionalLightComponent>())
        {
            auto &light = entity.GetComponent<DirectionalLightComponent>();
            m_Context->GetLights()->RemoveDirectionalLight();
        }
        // if entity is point light or spotlight, remove it from light list
        if (entity.HasComponent<PointLightComponent>())
        {
            auto &pointLight = entity.GetComponent<PointLightComponent>();
            m_Context->GetLights()->RemovePointLight(pointLight.Index);
        }
        if (entity.HasComponent<SpotLightComponent>())
        {
            auto &spotLight = entity.GetComponent<SpotLightComponent>();
            m_Context->GetLights()->RemoveSpotLight(spotLight.Index);
        }

        // remove from parent, if it is a child component
        auto parent = entity.GetComponent<ParentComponent>();
        if (parent.HasParent)
        {
            auto &pc = m_Context->GetEntityByUUID(parent.Parent).GetComponent<ParentComponent>();
            pc.RemoveChild(entity.GetComponent<IDComponent>().ID);
        }

        if (m_SelectionContext == entity) m_SelectionContext = {};
        // delete entity recursively
        m_Context->DestroyEntityRecursive(entity);
    }

    if (open)
    {
        std::vector<UUID> children = parentComponent.Children;
        for (auto &child : children)
        {
            auto entity = m_Context->GetEntityByUUID(child);
            DrawEntityNode(entity);
        }
        ImGui::TreePop();
    };
}

void SceneHierarchyPanel::DrawComponents(Entity entity)
{
    if (m_SelectionContext.HasComponent<TagComponent>())
    {
        auto &tag = m_SelectionContext.GetComponent<TagComponent>().Tag;
        char buffer[256];
        memset(buffer, 0, sizeof(buffer));
        strcpy(buffer, tag.c_str());

        if (ImGui::InputText(_labelPrefix("Tag"), buffer, sizeof(buffer)))
        {
            tag = std::string(buffer);
        }
    }

    if (entity.HasComponent<TransformComponent>())
    {
        _collapsingHeaderStyle();
        if (ImGui::CollapsingHeader("Transform"))
        {
            auto &transform = entity.GetComponent<TransformComponent>();
            if (ImGui::BeginPopupContextItem())
            {
                if (ImGui::MenuItem("Reset Transform"))
                {
                    transform.Translation = glm::vec3(0.0f);
                    transform.Rotation = glm::vec3(0.0f);
                    transform.Scale = glm::vec3(1.0f);
                }
                ImGui::EndPopup();
            }

            _drawVec3Control("Position", transform.Translation, 0.0f);
            _drawVec3Control("Rotation", transform.Rotation, 0.0f);
            _drawVec3Control("Scale", transform.Scale, 1.0f);
        }
    };

    if (entity.HasComponent<CameraComponent>())
    {
        ImGui::PushID("Camera");
        bool removeComponent = false;
        _collapsingHeaderStyle();
        if (ImGui::CollapsingHeader("Camera"))
        {
            REMOVABLE_COMPONENT
            auto &cameraComponent = entity.GetComponent<CameraComponent>();
            auto &camera = cameraComponent.Camera;

            if (ImGui::Checkbox(_labelPrefix("Primary"), &cameraComponent.Primary))
            {
                auto view = m_Context->m_Registry.view<CameraComponent>();
                for (auto entity : view)
                {
                    auto &otherCameraComponent = view.get<CameraComponent>(entity);
                    if (&otherCameraComponent != &cameraComponent) otherCameraComponent.Primary = false;
                }
            }

            float perspectiveVerticalFOV = glm::degrees(camera->GetPerspectiveVerticalFOV());
            if (ImGui::DragFloat(_labelPrefix("Vertical FOV"), &perspectiveVerticalFOV))
                camera->SetPerspectiveVerticalFOV(glm::radians(perspectiveVerticalFOV));

            float perspectiveNearClip = camera->GetPerspectiveNearClip();
            if (ImGui::DragFloat(_labelPrefix("Near Clip"), &perspectiveNearClip))
                camera->SetPerspectiveNearClip(perspectiveNearClip);

            float perspectiveFarClip = camera->GetPerspectiveFarClip();
            if (ImGui::DragFloat(_labelPrefix("Far Clip"), &perspectiveFarClip))
                camera->SetPerspectiveFarClip(perspectiveFarClip);

            // preview camera
            if (ImGui::TreeNode("Preview", "Preview"))
            {
                auto &cameraComponent = entity.GetComponent<CameraComponent>();
                auto &camera = cameraComponent.Camera;
                camera->ResizeFramebuffer({ImGui::GetContentRegionAvail().x - 20, 980});

                ImGui::Image((void *)(uint64_t)camera->GetPreviewTexture(m_Context.get())->GetRendererID(),
                             ImVec2(ImGui::GetContentRegionAvail().x - 20, 420));

                ImGui::TreePop();
            }
        }

        if (removeComponent) entity.RemoveComponent<CameraComponent>();
        ImGui::PopID();
    }

    if (entity.HasComponent<StaticMeshComponent>() || entity.HasComponent<SkinnedMeshComponent>())
    {
        bool removeComponent = false;
        MeshComponent *entityComponent = new MeshComponent();

		if (entity.HasComponent<StaticMeshComponent>()) entityComponent = &entity.GetComponent<StaticMeshComponent>();
        else if (entity.HasComponent<SkinnedMeshComponent>()) entityComponent = &entity.GetComponent<SkinnedMeshComponent>();

        _collapsingHeaderStyle();
        if (ImGui::CollapsingHeader("Mesh"))
        {
            auto meshName = AssetManager::GetAssetName(entityComponent->Handle);
            REMOVABLE_COMPONENT
            ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.0f, 0.5f));

            if (ImGui::Button(_labelPrefix("Mesh", meshName.c_str()), ImVec2(-1, 35)))
            {
                ImGui::OpenPopup("MeshSelector");
            }

            if (ImGui::BeginPopup("MeshSelector"))
            {
                // search bar
                static char searchStr[128] = "";
                ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5, 5));
                ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 10.0f);
                ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.05f, 0.05f, 0.05f, 0.54f));
                if (ImGui::InputTextWithHint("##Search", ICON_FA_SEARCH "  Search", searchStr, IM_ARRAYSIZE(searchStr)))
                {
                    // Search(searchStr);
                }
                ImGui::PopStyleColor();
                ImGui::PopStyleVar(2);

                ImGui::Spacing();

                // list of meshes
                ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5, 5));
                ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 10.0f);
                ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.05f, 0.05f, 0.05f, 0.54f));
                ImGui::BeginChild("MeshList", ImVec2(ImGui::GetContentRegionAvail().x, 300), true);
                {
                    auto assets = AssetManager::GetRegistry();
                    for (auto &asset : assets)
                    {
                        if (asset.second.Type != AssetType::Mesh) continue;

                        auto assetName = AssetManager::GetAssetName(asset.first);

                        if (ImGui::Selectable(assetName.c_str(), asset.first == entityComponent->Handle))
                        {
                            entityComponent->Handle = asset.first;
                            entityComponent->MaterialHandle = 0;
                            // close popup
                            ImGui::CloseCurrentPopup();
                        }
                    }
                    ImGui::EndChild();
                }
                ImGui::PopStyleColor();
                ImGui::PopStyleVar(2);

                ImGui::EndPopup();
            }

            ImGui::PopStyleVar();
            if (ImGui::BeginDragDropTarget())
            {
                if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
                {
                    const char *path = (const char *)payload->Data;
                    if (Utils::GetAssetTypeFromExtension(path) == AssetType::Mesh)
                    {
                        entityComponent->Handle = AssetManager::GetAssetHandleFromPath(path);
                    }
                }
                ImGui::EndDragDropTarget();
            }

            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.05f, 0.05f, 0.05f, 0.54f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.05f, 0.05f, 0.05f, 0.54f));

            auto asset = AssetManager::GetAsset<Mesh>(entityComponent->Handle);   
            if (ImGui::TreeNodeEx((void *)typeid(MeshComponent).hash_code(), 0, "Material"))
            {
                for (const auto &mesh : asset->StaticMeshes)
                {
                    bool openModal = false;
                    MaterialRef material = AssetManager::GetAsset<Material>(entityComponent->MaterialHandle);
                    if (material == nullptr)
                    {
                        material = mesh.DefaultMaterial;
                    }

                    ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.0f, 0.5f));
                    ImGui::Button(_labelPrefix("Name", material->Name.c_str()), ImVec2(-1, 0));
                    ImGui::PopStyleVar();

                    if (ImGui::BeginPopupContextItem())
                    {
                        if (material != mesh.DefaultMaterial &&
                            ImGui::MenuItem(ICON_FA_EDIT "   Open Material Editor"))
                        {
                            MaterialEditorPanel::OpenMaterialEditor(mesh.DefaultMaterial, true);
                        }
                        // remove
                        if (ImGui::MenuItem(ICON_FA_TRASH "   Remove"))
                        {
                            entityComponent->MaterialHandle = 0;
                        }
                        ImGui::EndPopup();
                    }

                    if (ImGui::BeginDragDropTarget())
                    {
                        if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
                        {
                            const char *path = (const char *)payload->Data;
                            if (Utils::GetAssetTypeFromExtension(path) == AssetType::Material)
                            {
                                auto handle = AssetManager::GetAssetHandleFromPath(path);
                                // model->SetMaterialHandle(i, handle);
                                entityComponent->MaterialHandle = handle;
                            }
                        }
                        ImGui::EndDragDropTarget();
                    }
                }
                
                ImGui::TreePop();
			}

            if (removeComponent) entity.RemoveComponent<MeshComponent>();
            ImGui::PopStyleColor(2);
        }

        if (entity.HasComponent<VisibilityComponent>())
        {
            _collapsingHeaderStyle();
            if (ImGui::CollapsingHeader("Visibility"))
            {
                auto &entityComponent = entity.GetComponent<VisibilityComponent>();
                ImGui::Checkbox(_labelPrefix("Visibility"), &entityComponent.IsVisible);
            }
        }
    }

    if (entity.HasComponent<DirectionalLightComponent>())
    {
        bool removeComponent = false;
        _collapsingHeaderStyle();
        if (ImGui::CollapsingHeader("DirectionalLight"))\
        {
            REMOVABLE_COMPONENT
            auto &entityComponent = entity.GetComponent<DirectionalLightComponent>();
            auto &transform = entity.GetComponent<TransformComponent>();
            entityComponent.Light.Direction = transform.Rotation;
            ImGui::Checkbox(_labelPrefix("Enabled"), &entityComponent.Enabled);
            ImGui::ColorEdit3(_labelPrefix("Color"), glm::value_ptr(entityComponent.Light.Color));
            ImGui::DragFloat(_labelPrefix("Intensity"), &entityComponent.Light.Intensity, 0.1f, 0.0f, 10000.0f);
        }
        if (removeComponent) entity.RemoveComponent<DirectionalLightComponent>();
    }

    if (entity.HasComponent<PointLightComponent>())
    {
        bool removeComponent = false;
        _collapsingHeaderStyle();
        if (ImGui::CollapsingHeader("PointLight"))
        {
            REMOVABLE_COMPONENT
            auto &entityComponent = entity.GetComponent<PointLightComponent>();
            auto &transform = entity.GetComponent<TransformComponent>();
            entityComponent.Light.Position = transform.Translation;
            ImGui::Checkbox(_labelPrefix("Enabled"), &entityComponent.Enabled);
            ImGui::ColorEdit3(_labelPrefix("Color"), glm::value_ptr(entityComponent.Light.Color));
            ImGui::DragFloat(_labelPrefix("Intensity"), &entityComponent.Light.Intensity, 0.1f, 0.0f, 10000.0f);
        }
        if (removeComponent) entity.RemoveComponent<PointLightComponent>();
    }

    if (entity.HasComponent<NetScriptComponent>())
    {
        _collapsingHeaderStyle();
        if (ImGui::CollapsingHeader(".NET Script"))
        {
            auto &entityComponent = entity.GetComponent<NetScriptComponent>();
            auto netScript = AssetManager::GetAsset<NetScript>(entityComponent.Handle);
            auto moduleName = netScript ? netScript->GetClassName() : "None";

            ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.0f, 0.5f));
            ImGui::Button(_labelPrefix("Module", moduleName.c_str()), ImVec2(-1, 0));
            ImGui::PopStyleVar();
            if (ImGui::BeginDragDropTarget())
            {
                if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
                {
                    auto path = static_cast<const char *>(payload->Data);
                    if (Utils::GetAssetTypeFromExtension(path) == AssetType::NetScript)
                    {
                        entityComponent.Handle = AssetManager::GetAssetHandleFromPath(path);
                    }
                }
                ImGui::EndDragDropTarget();
            }
        }
    }

    if (entity.HasComponent<SpotLightComponent>())
    {
        bool removeComponent = false;
        _collapsingHeaderStyle();
        if (ImGui::CollapsingHeader("SpotLight"))
        {
            REMOVABLE_COMPONENT
            auto &entityComponent = entity.GetComponent<SpotLightComponent>();
            auto &transform = entity.GetComponent<TransformComponent>();
            entityComponent.Light.Position = transform.Translation;
            entityComponent.Light.Direction = transform.Rotation;
            ImGui::Checkbox(_labelPrefix("Enabled"), &entityComponent.Enabled);
            ImGui::ColorEdit3(_labelPrefix("Color"), glm::value_ptr(entityComponent.Light.Color));
            ImGui::DragFloat(_labelPrefix("Cutoff"), &entityComponent.Light.Cutoff, 0.1f, 0.0f, 90.0f);
            ImGui::DragFloat(_labelPrefix("Outer Cutoff"), &entityComponent.Light.OuterCutoff, 0.1f, 0.0f, 90.0f);
            ImGui::DragFloat(_labelPrefix("Intensity"), &entityComponent.Light.Intensity, 0.1f, 0.0f, 10000.0f);
        }
        if (removeComponent) entity.RemoveComponent<SpotLightComponent>();
    }

    // Physics Stuff
    if (entity.HasComponent<RigidBodyComponent>())
    {
        bool removeComponent = false;
        _collapsingHeaderStyle();
        if (ImGui::CollapsingHeader("Rigid Body"))
        {
            REMOVABLE_COMPONENT
            // drop down menu for motion type
            auto &entityComponent = entity.GetComponent<RigidBodyComponent>();
            auto &transform = entity.GetComponent<TransformComponent>();
            if (ImGui::BeginCombo(_labelPrefix("Motion Type"),
                                  Physics::MotionTypeToString(entityComponent.MotionType).c_str()))
            {
                for (int i = 0; i < 2; i++)
                {
                    bool isSelected = Physics::MotionTypeToString(entityComponent.MotionType) ==
                                      Physics::MotionTypeToString((Physics::MotionType)i);
                    if (ImGui::Selectable(Physics::MotionTypeToString((Physics::MotionType)i).c_str(), isSelected))
                    {
                        entityComponent.MotionType = static_cast<Physics::MotionType>(i);
                    }
                    if (isSelected) ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }
            if (entityComponent.MotionType == Physics::MotionType::Dynamic)
            {
                ImGui::DragFloat(_labelPrefix("Mass"), &entityComponent.Mass, 0.1f, 0.0f, 10000.0f);
                ImGui::DragFloat(_labelPrefix("Linear Damping"), &entityComponent.LinearDamping, 0.01f, 0.0f, 1.0f);
                ImGui::DragFloat(_labelPrefix("Angular Damping"), &entityComponent.AngularDamping, 0.01f, 0.0f, 1.0f);
                ImGui::Checkbox(_labelPrefix("Use Gravity"), &entityComponent.UseGravity);
                ImGui::Checkbox(_labelPrefix("Is Kinematic"), &entityComponent.IsKinematic);
                // constraints tree node
                if (ImGui::TreeNodeEx((void *)typeid(RigidBodyComponent).hash_code(), ImGuiTreeNodeFlags_DefaultOpen,
                                      "Constraints"))
                {
                    ImGui::TreePop();
                }
            }
        }
        if (removeComponent) entity.RemoveComponent<RigidBodyComponent>();
    }

    if (entity.HasComponent<BoxColliderComponent>())
    {
        bool removeComponent = false;
        _collapsingHeaderStyle();
        if (ImGui::CollapsingHeader("Box Collider"))
        {
            REMOVABLE_COMPONENT
            auto &entityComponent = entity.GetComponent<BoxColliderComponent>();
            _drawVec3Control("Size", entityComponent.Size, 0.0f);
            ImGui::Checkbox(_labelPrefix("Is Trigger"), &entityComponent.IsTrigger);
        }
        if (removeComponent) entity.RemoveComponent<BoxColliderComponent>();
    }

    if (entity.HasComponent<SphereColliderComponent>())
    {
        bool removeComponent = false;
        _collapsingHeaderStyle();
        if (ImGui::CollapsingHeader("Sphere Collider"))
        {
            REMOVABLE_COMPONENT
            auto &entityComponent = entity.GetComponent<SphereColliderComponent>();
            ImGui::DragFloat(_labelPrefix("Radius"), &entityComponent.Radius, 0.1f, 0.0f, 10000.0f);
            ImGui::Checkbox(_labelPrefix("Is Trigger"), &entityComponent.IsTrigger);
        }
        if (removeComponent) entity.RemoveComponent<SphereColliderComponent>();
    }
}

Entity SceneHierarchyPanel::CreateEntityPopup()
{
    Entity entity;
    if (ImGui::MenuItem(ICON_FA_CUBE "   Create Empty Entity"))
    {
        entity = m_Context->CreateEntity("Empty Entity");
        m_SelectionContext = entity;
        return entity;
    }

    ImGui::Spacing();
    ImGui::Separator();

    if (ImGui::MenuItem(ICON_FA_VIDEO "  Camera"))
    {
        entity = m_Context->CreateEntity("Camera");
        entity.AddComponent<CameraComponent>();
        m_SelectionContext = entity;
        return entity;
    }
    ImGui::Spacing();
    ImGui::Separator();

    // check if scene does not contain directional light entity
    auto dLight = m_Context->GetEntity("Directional Light");
    if (dLight == nullptr)
    {
        if (ImGui::MenuItem(ICON_FA_SUN "  Directional Light"))
        {
            entity = m_Context->CreateEntity("Directional Light");
            entity.AddComponent<DirectionalLightComponent>();
            m_SelectionContext = entity;
            return entity;
        }
    }
    if (ImGui::MenuItem(ICON_FA_LIGHTBULB "  Point Light"))
    {
        static int count = 1;
        auto num = m_Context->GetLights()->GetNumPointLights() + 1;
        entity = m_Context->CreateEntity("Point Light");
        auto &pc = entity.AddComponent<PointLightComponent>();
        pc.Index = count++;
        m_SelectionContext = entity;
        return entity;
    }
    if (ImGui::MenuItem(ICON_FA_LIGHTBULB "  Spot Light"))
    {
        static int count = 1;
        auto num = m_Context->GetLights()->GetNumSpotLights() + 1;
        entity = m_Context->CreateEntity("Spot Light " + std::to_string(num));
        auto &sc = entity.AddComponent<SpotLightComponent>();
        sc.Index = count++;
        m_SelectionContext = entity;
        return entity;
    }
    ImGui::Spacing();
    ImGui::Separator();

    if (ImGui::BeginMenu(ICON_FA_CUBE "  Mesh"))
    {
        if (ImGui::MenuItem(ICON_FA_CUBE "  Empty Mesh"))
        {
            entity = m_Context->CreateEntity("Mesh");
            entity.AddComponent<MeshComponent>();
            m_SelectionContext = entity;
            ImGui::EndPopup();
            return entity;
        }
        if (ImGui::MenuItem(ICON_FA_SQUARE "  Cube"))
        {
            ImGui::EndPopup();
        }
        if (ImGui::MenuItem(ICON_FA_CIRCLE "  Sphere"))
        {
            ImGui::EndPopup();
        }
        if (ImGui::MenuItem(ICON_FA_CAPSULES "  Capsule"))
        {
            ImGui::EndPopup();
        }
        ImGui::EndPopup();
    }

    return Entity{entt::null, m_Context.get()};
}

void SceneHierarchyPanel::SetContext(const std::shared_ptr<Scene> &context)
{
    m_Context = context;
    m_SelectionContext = {};
}

void SceneHierarchyPanel::SetSelectedEntity(Entity entity)
{
    m_SelectionContext = entity;
    m_Context->SetSelectedEntity(entity);
}

void SceneHierarchyPanel::OnImGuiRender()
{

    ImGui::Begin("Scene Hierarchy");
    if (m_Context)
    {
        if (ImGui::Button(ICON_FA_PLUS)) ImGui::OpenPopup("addEntity");
        ImGui::SameLine();

        ImGui::PushItemWidth(-1.0f);
        static char searchStr[128] = "";
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5, 5));
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 10.0f);
        ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.05f, 0.05f, 0.05f, 0.54f));
        if (ImGui::InputTextWithHint("##Search", ICON_FA_SEARCH "  Search", searchStr, IM_ARRAYSIZE(searchStr)))
        {
            // Search(searchStr);
        }
        ImGui::PopStyleColor();
        ImGui::PopStyleVar(2);
        ImGui::PopItemWidth();

        ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.05f, 0.05f, 0.05f, 0.54f));
        ImGui::BeginChild("SceneHierarchy", ImVec2(0, 0), true);

        // Right-click on blank space
        if (ImGui::BeginPopupContextWindow())
        {
            CreateEntityPopup();
            ImGui::EndPopup();
        }

        if (ImGui::BeginPopup("addEntity"))
        {
            CreateEntityPopup();
            ImGui::EndPopup();
        }

        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10);
        // ImGui::Separator();
        // ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10);

        // Add padding around the list
        m_Context->m_Registry.each(
            [this](auto entityId)
            {
                Entity entity{entityId, m_Context.get()};
                if (!entity.GetComponent<ParentComponent>().HasParent) DrawEntityNode(entity);
            });

        ImGui::EndChild();
        ImGui::PopStyleColor();
    }
    ImGui::End();

    ImGui::Begin("Properties");
    if (m_SelectionContext)
    {
        DrawComponents(m_SelectionContext);

        ImGui::Spacing();
        ImGui::Spacing();
        ImGui::Spacing();

        float buttonWidth = 260.0f; // Adjust padding as needed
        ImGui::SetCursorPosX((ImGui::GetContentRegionMax().x / 2.0f + buttonWidth / 2.0f) - buttonWidth);

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.2f, 0.2f, 0.5f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.3f, 0.3f, 0.6f));
        ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(0, 5.0f));
        if (ImGui::Button(ICON_FA_PLUS "   Add Component", ImVec2(buttonWidth, 0))) ImGui::OpenPopup("AddComponent");
        ImGui::PopStyleVar();
        ImGui::PopStyleColor(2);

        // TODO: refactor this so you dont have to manually list the components
        if (ImGui::BeginPopup("AddComponent"))
        {
            ADD_COMPONENT_MENU(RigidBodyComponent, ICON_FA_CUBE "   Rigid Body");
            ADD_COMPONENT_MENU(BoxColliderComponent, ICON_FA_CUBE "   Box Collider");
            ADD_COMPONENT_MENU(SphereColliderComponent, ICON_FA_CIRCLE "   Sphere Collider");
            ADD_COMPONENT_MENU(CameraComponent, ICON_FA_VIDEO "   Camera");
            ADD_COMPONENT_MENU(MeshComponent, ICON_FA_CUBE "   Mesh");
            ADD_COMPONENT_MENU(DirectionalLightComponent, ICON_FA_SUN "   Directional Light");
            ADD_COMPONENT_MENU(PointLightComponent, ICON_FA_LIGHTBULB "   Point Light");
            ADD_COMPONENT_MENU(SpotLightComponent, ICON_FA_LIGHTBULB "   Spot Light");
            ADD_COMPONENT_MENU(NetScriptComponent, ICON_FA_CODE "   .NET Script");

            ImGui::EndPopup();
        }
    }
    ImGui::End();
}
} // namespace Engine
