#include "SceneHierarchyPanel.h"

#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/glm.hpp>

#include "Light.h"
#include "MaterialEditorPanel.h"
#include "ImGuiHelpers.h"
#include "PhysicsComponents.h"
#include "MeshImporter.h"
#include "PlatformUtils.h"

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

#define CREATE_BUILTIN_MESH(name, filepath)                                                                            \
    auto entity = m_Context->CreateEntity(name);                                                                       \
    entity.AddComponent<MeshComponent>();                                                                              \
    auto &model = entity.GetComponent<MeshComponent>();                                                                \
    model.ModelResource = MeshImporter::LoadModel(filepath);                                                           \
    m_SelectionContext = entity;

#define REMOVABLE_COMPONENT                                                                                            \
    if (ImGui::BeginPopupContextItem())                                                                                \
    {                                                                                                                  \
        if (ImGui::MenuItem("Remove Component")) removeComponent = true;                                               \
        ImGui::EndPopup();                                                                                             \
    }

namespace Engine
{
SceneHierarchyPanel::SceneHierarchyPanel(const std::shared_ptr<Scene> &context)
{
    //
    SetContext(context);
}

void SceneHierarchyPanel::DrawEntityNode(Entity entity)
{
    const auto &tag = entity.GetComponent<TagComponent>().Tag;

    auto flags = ((m_SelectionContext == entity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
    if (ImGui::TreeNodeEx((void *)(uint64_t)(uint32_t)entity, flags, "%s", tag.c_str()))
    {
        ImGui::TreePop();
    };
    // ImGui::Selectable(tag.c_str(), m_SelectionContext == entity);
    if (ImGui::IsItemClicked()) m_SelectionContext = entity;

    bool entityDeleted = false;
    if (ImGui::BeginPopupContextItem())
    {
        if (ImGui::MenuItem("Delete Entity")) entityDeleted = true;
        ImGui::EndPopup();
    }

    if (entityDeleted)
    {
        if (entity.HasComponent<DirectionalLightComponent>())
        {
            auto &light = entity.GetComponent<DirectionalLightComponent>();
            Light::RemoveDirectionalLight();
        }
        // if entity is point light or spot light, remove it from light list
        if (entity.HasComponent<PointLightComponent>())
        {
            auto &pointLight = entity.GetComponent<PointLightComponent>();
            Light::RemovePointLight(pointLight.Index);
        }
        if (entity.HasComponent<SpotLightComponent>())
        {
            auto &spotLight = entity.GetComponent<SpotLightComponent>();
            Light::RemoveSpotLight(spotLight.Index);
        }

        if (m_SelectionContext == entity) m_SelectionContext = {};
        m_Context->DestroyEntity(entity);
    }
}

void SceneHierarchyPanel::DrawComponents(Entity entity)
{

    if (entity.HasComponent<TransformComponent>())
    {
        if (ImGui::TreeNodeEx((void *)typeid(TransformComponent).hash_code(), ImGuiTreeNodeFlags_DefaultOpen,
                              "Transform"))
        {
            auto &translation = entity.GetComponent<TransformComponent>().Translation;
            _drawVec3Control("Position", translation, 0.0f);

            auto &rotation = entity.GetComponent<TransformComponent>().Rotation;
            _drawVec3Control("Rotation", rotation, 0.0f);

            auto &scale = entity.GetComponent<TransformComponent>().Scale;
            _drawVec3Control("Scale", scale, 1.0f);

            ImGui::TreePop();
        }
    }

    if (entity.HasComponent<CameraComponent>())
    {
        bool removeComponent = false;
        if (ImGui::TreeNodeEx((void *)typeid(CameraComponent).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, "Camera"))
        {
            REMOVABLE_COMPONENT
            auto &cameraComponent = entity.GetComponent<CameraComponent>();
            auto &camera = cameraComponent.Camera;

            if (ImGui::Checkbox(_labelPrefix("Primary").c_str(), &cameraComponent.Primary))
            {
                auto view = m_Context->m_Registry.view<CameraComponent>();
                for (auto entity : view)
                {
                    auto &otherCameraComponent = view.get<CameraComponent>(entity);
                    if (&otherCameraComponent != &cameraComponent) otherCameraComponent.Primary = false;
                }
            }

            float perspectiveVerticalFOV = glm::degrees(camera.GetPerspectiveVerticalFOV());
            if (ImGui::DragFloat(_labelPrefix("Vertical FOV").c_str(), &perspectiveVerticalFOV))
                camera.SetPerspectiveVerticalFOV(glm::radians(perspectiveVerticalFOV));

            float perspectiveNearClip = camera.GetPerspectiveNearClip();
            if (ImGui::DragFloat(_labelPrefix("Near Clip").c_str(), &perspectiveNearClip))
                camera.SetPerspectiveNearClip(perspectiveNearClip);

            float perspectiveFarClip = camera.GetPerspectiveFarClip();
            if (ImGui::DragFloat(_labelPrefix("Far Clip").c_str(), &perspectiveFarClip))
                camera.SetPerspectiveFarClip(perspectiveFarClip);

            ImGui::TreePop();
        }

        if (removeComponent) entity.RemoveComponent<CameraComponent>();
    }

    if (entity.HasComponent<MeshComponent>())
    {
        bool removeComponent = false;
        auto &entityComponent = entity.GetComponent<MeshComponent>();
        if (ImGui::TreeNodeEx((void *)typeid(MeshComponent).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, "Mesh"))
        {
            REMOVABLE_COMPONENT
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.05f, 0.05f, 0.05f, 0.54f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.05f, 0.05f, 0.05f, 0.54f));
            ImGui::Button(_labelPrefix("Mesh", "Untitled").c_str(), ImVec2(ImGui::GetContentRegionAvail().x, 0.0f));
            ImGui::PopStyleColor(2);
            if (ImGui::BeginDragDropTarget())
            {
                if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
                {
                    // TODO: check if payload is a handle or a path, if path, convert to handle by loading asset
                    const char *handle = (const char *)payload->Data;
                    // convert handle to uint64_t(AssetHandle)
                    entityComponent.Handle = std::stoull(handle);
                }
                ImGui::EndDragDropTarget();
            }

            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.05f, 0.05f, 0.05f, 0.54f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.05f, 0.05f, 0.05f, 0.54f));

            auto modelEntity = entity.GetComponent<MeshComponent>();
            const auto &model = modelEntity.ModelResource ? modelEntity.ModelResource
                                                          : AssetManager::GetAsset<Model>(entityComponent.Handle);
            const auto &material = AssetManager::GetAsset<Material>(entityComponent.MaterialHandle);
            if (model != nullptr)
            {
                for (const auto &mesh : model->GetMeshes())
                {
                    bool openModal = false;
                    if (ImGui::Button(_labelPrefix("Material", "Untitled").c_str(),
                                      ImVec2(ImGui::GetContentRegionAvail().x, 0.0f)))
                        openModal = true;

                    if (openModal) ImGui::OpenPopup("material_popup");

                    if (ImGui::BeginPopup("material_popup"))
                    {
                        // TODO: check if payload is a handle or a path, if path, convert to handle by loading asset
                        if (ImGui::MenuItem(ICON_FA_TRASH_RESTORE "   Remove")) entityComponent.MaterialHandle = 0;
                        if (ImGui::MenuItem(ICON_FA_EDIT "   Open Material Editor"))
                            MaterialEditorPanel::OpenMaterialEditor(entityComponent.MaterialHandle);
                        ImGui::EndPopup();
                    }

                    if (ImGui::BeginDragDropTarget())
                    {
                        if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
                        {
                            const char *handle = (const char *)payload->Data;
                            // convert handle to uint64_t(AssetHandle)
                            entityComponent.MaterialHandle = std::stoull(handle);
                        }
                        ImGui::EndDragDropTarget();
                    }
                }
            }

            if (removeComponent) entity.RemoveComponent<MeshComponent>();
            ImGui::PopStyleColor(2);
            ImGui::TreePop();
        }

        if (entity.HasComponent<VisibilityComponent>())
        {
            if (ImGui::TreeNodeEx((void *)typeid(VisibilityComponent).hash_code(), ImGuiTreeNodeFlags_DefaultOpen,
                                  "Visibility"))
            {
                auto &entityComponent = entity.GetComponent<VisibilityComponent>();
                ImGui::Checkbox(_labelPrefix("Visibility").c_str(), &entityComponent.IsVisible);
                ImGui::TreePop();
            }
        }
    }

    if (entity.HasComponent<SkyLightComponent>())
    {
        bool removeComponent = false;
        if (ImGui::TreeNodeEx((void *)typeid(SkyLightComponent).hash_code(), ImGuiTreeNodeFlags_DefaultOpen,
                              "SkyLight"))
        {
            REMOVABLE_COMPONENT
            auto &component = entity.GetComponent<SkyLightComponent>();
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.05f, 0.05f, 0.05f, 0.54f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.05f, 0.05f, 0.05f, 0.54f));
            ImGui::Button(_labelPrefix("Sky Light", "Environment").c_str(), ImVec2(430.0f, 30.0f));
            ImGui::PopStyleColor(2);
            if (ImGui::BeginDragDropTarget())
            {
                if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
                {
                    const char *handleStr = (const char *)payload->Data;
                    AssetHandle handle = std::stoull(handleStr);
                    component.Use(handle, 2048);
                }
                ImGui::EndDragDropTarget();
            }
            ImGui::TreePop();
        }
        if (removeComponent) entity.RemoveComponent<SkyLightComponent>();
    }

    if (entity.HasComponent<DirectionalLightComponent>())
    {
        bool removeComponent = false;
        if (ImGui::TreeNodeEx((void *)typeid(DirectionalLightComponent).hash_code(), ImGuiTreeNodeFlags_DefaultOpen,
                              "DirectionalLight"))
        {
            REMOVABLE_COMPONENT
            auto &entityComponent = entity.GetComponent<DirectionalLightComponent>();
            auto &transform = entity.GetComponent<TransformComponent>();
            entityComponent.Light.Direction = transform.Rotation;
            ImGui::ColorEdit3(_labelPrefix("Color").c_str(), glm::value_ptr(entityComponent.Light.Color));
            ImGui::DragFloat(_labelPrefix("Intensity").c_str(), &entityComponent.Light.Intensity, 1.0f, 0.0f, 10000.0f);

            Light::SetDirectionalLight(&entityComponent.Light);

            ImGui::TreePop();
        }
        if (removeComponent) entity.RemoveComponent<DirectionalLightComponent>();
    }

    if (entity.HasComponent<PointLightComponent>())
    {
        bool removeComponent = false;
        if (ImGui::TreeNodeEx((void *)typeid(PointLightComponent).hash_code(), ImGuiTreeNodeFlags_DefaultOpen,
                              "PointLight"))
        {
            REMOVABLE_COMPONENT
            auto &entityComponent = entity.GetComponent<PointLightComponent>();
            auto &transform = entity.GetComponent<TransformComponent>();
            entityComponent.Light.Position = transform.Translation;
            ImGui::ColorEdit3(_labelPrefix("Color").c_str(), glm::value_ptr(entityComponent.Light.Color));
            ImGui::DragFloat(_labelPrefix("Intensity").c_str(), &entityComponent.Light.Intensity, 1.0f, 0.0f, 10000.0f);

            Light::SetPointLight(entityComponent.Light, entityComponent.Index);

            ImGui::TreePop();
        }
        if (removeComponent) entity.RemoveComponent<PointLightComponent>();
    }

    if (entity.HasComponent<SpotLightComponent>())
    {
        bool removeComponent = false;
        if (ImGui::TreeNodeEx((void *)typeid(SpotLightComponent).hash_code(), ImGuiTreeNodeFlags_DefaultOpen,
                              "SpotLight"))
        {
            REMOVABLE_COMPONENT
            auto &entityComponent = entity.GetComponent<SpotLightComponent>();
            auto &transform = entity.GetComponent<TransformComponent>();
            entityComponent.Light.Position = transform.Translation;
            entityComponent.Light.Direction = transform.Rotation;
            ImGui::ColorEdit3(_labelPrefix("Color").c_str(), glm::value_ptr(entityComponent.Light.Color));
            ImGui::DragFloat(_labelPrefix("Cutoff").c_str(), &entityComponent.Light.Cutoff, 0.1f, 0.0f, 90.0f);
            ImGui::DragFloat(_labelPrefix("Outer Cutoff").c_str(), &entityComponent.Light.OuterCutoff, 0.1f, 0.0f,
                             90.0f);
            ImGui::DragFloat(_labelPrefix("Intensity").c_str(), &entityComponent.Light.Intensity, 1.0f, 0.0f, 10000.0f);

            Light::SetSpotLight(entityComponent.Light, entityComponent.Index);

            ImGui::TreePop();
        }
        if (removeComponent) entity.RemoveComponent<SpotLightComponent>();
    }

    // Physics Stuff
    if (entity.HasComponent<RigidBodyComponent>())
    {
        bool removeComponent = false;
        if (ImGui::TreeNodeEx((void *)typeid(RigidBodyComponent).hash_code(), ImGuiTreeNodeFlags_DefaultOpen,
                              "Rigid Body"))
        {
            REMOVABLE_COMPONENT
            // drop down menu for motion type
            auto &entityComponent = entity.GetComponent<RigidBodyComponent>();
            auto &transform = entity.GetComponent<TransformComponent>();
            if (ImGui::BeginCombo(_labelPrefix("Motion Type").c_str(),
                                  Physics::MotionTypeToString(entityComponent.MotionType).c_str()))
            {
                for (int i = 0; i < 2; i++)
                {
                    bool isSelected = Physics::MotionTypeToString(entityComponent.MotionType) ==
                                      Physics::MotionTypeToString((Physics::MotionType)i);
                    if (ImGui::Selectable(Physics::MotionTypeToString((Physics::MotionType)i).c_str(), isSelected))
                    {
                        entityComponent.MotionType = (Physics::MotionType)i;
                    }
                    if (isSelected) ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }
            if (entityComponent.MotionType == Physics::MotionType::Dynamic)
            {
                ImGui::DragFloat(_labelPrefix("Mass").c_str(), &entityComponent.Mass, 0.1f, 0.0f, 10000.0f);
                ImGui::DragFloat(_labelPrefix("Linear Damping").c_str(), &entityComponent.LinearDamping, 0.01f, 0.0f,
                                 1.0f);
                ImGui::DragFloat(_labelPrefix("Angular Damping").c_str(), &entityComponent.AngularDamping, 0.01f, 0.0f,
                                 1.0f);
                ImGui::Checkbox(_labelPrefix("Use Gravity").c_str(), &entityComponent.UseGravity);
                ImGui::Checkbox(_labelPrefix("Is Kinematic").c_str(), &entityComponent.IsKinematic);
                // constraints tree node
                if (ImGui::TreeNodeEx((void *)typeid(RigidBodyComponent).hash_code(), ImGuiTreeNodeFlags_DefaultOpen,
                                      "Constraints"))
                {
                    ImGui::TreePop();
                }
            }
            ImGui::TreePop();
        }
        if (removeComponent) entity.RemoveComponent<RigidBodyComponent>();
    }

    if (entity.HasComponent<BoxColliderComponent>())
    {
        bool removeComponent = false;
        if (ImGui::TreeNodeEx((void *)typeid(BoxColliderComponent).hash_code(), ImGuiTreeNodeFlags_DefaultOpen,
                              "Box Collider"))
        {
            REMOVABLE_COMPONENT
            auto &entityComponent = entity.GetComponent<BoxColliderComponent>();
            _drawVec3Control("Size", entityComponent.Size, 0.0f);
            ImGui::Checkbox(_labelPrefix("Is Trigger").c_str(), &entityComponent.IsTrigger);
            ImGui::TreePop();
        }
        if (removeComponent) entity.RemoveComponent<BoxColliderComponent>();
    }
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
        // Right-click on blank space
        if (ImGui::BeginPopupContextWindow())
        {
            if (ImGui::MenuItem(ICON_FA_CUBE "   Create Empty Entity")) m_Context->CreateEntity("Empty Entity");

            if (ImGui::BeginMenu(ICON_FA_PLUS "   Add Entity"))
            {
                if (ImGui::MenuItem(ICON_FA_VIDEO "  Camera"))
                {
                    auto entity = m_Context->CreateEntity("Camera");
                    entity.AddComponent<CameraComponent>();
                    m_SelectionContext = entity;
                }
                ImGui::Spacing();
                ImGui::Separator();

                if (ImGui::MenuItem(ICON_FA_CLOUD_MOON "  Sky Light"))
                {
                    auto entity = m_Context->CreateEntity("Sky Light");
                    entity.AddComponent<SkyLightComponent>();
                    entity.RemoveComponent<TransformComponent>();
                    m_SelectionContext = entity;
                }
                ImGui::Separator();

                // check if scene does not contain directional light entity
                auto dLight = m_Context->GetEntity("Directional Light");
                if (dLight == nullptr)
                {
                    if (ImGui::MenuItem(ICON_FA_SUN "  Directional Light"))
                    {
                        auto entity = m_Context->CreateEntity("Directional Light");
                        entity.AddComponent<DirectionalLightComponent>();
                        m_SelectionContext = entity;
                    }
                }
                if (ImGui::MenuItem(ICON_FA_LIGHTBULB "  Point Light"))
                {
                    auto num = Light::GetNumPointLights() + 1;
                    auto entity = m_Context->CreateEntity("Point Light " + std::to_string(num));
                    entity.AddComponent<PointLightComponent>();
                    m_SelectionContext = entity;
                }
                if (ImGui::MenuItem(ICON_FA_LIGHTBULB "  Spot Light"))
                {
                    auto num = Light::GetNumSpotLights() + 1;
                    auto entity = m_Context->CreateEntity("Spot Light " + std::to_string(num));
                    entity.AddComponent<SpotLightComponent>();
                    m_SelectionContext = entity;
                }
                ImGui::Spacing();
                ImGui::Separator();

                if (ImGui::BeginMenu(ICON_FA_CUBE "  Mesh"))
                {
                    if (ImGui::MenuItem(ICON_FA_CUBE "  Empty Mesh"))
                    {
                        auto entity = m_Context->CreateEntity(" Mesh");
                        entity.AddComponent<MeshComponent>();
                        m_SelectionContext = entity;
                    }
                    if (ImGui::MenuItem(ICON_FA_SQUARE "  Cube"))
                    {
                        CREATE_BUILTIN_MESH("Cube", Utils::Path::GetAbsolute("/Resources/Models/Cube/scene.gltf"));
                    }
                    if (ImGui::MenuItem(ICON_FA_CIRCLE "  Sphere"))
                    {
                        CREATE_BUILTIN_MESH("Cube", Utils::Path::GetAbsolute("/Resources/Models/Sphere/untitled.fbx"));
                    }
                    if (ImGui::MenuItem(ICON_FA_CAPSULES "  Capsule"))
                    {
                        CREATE_BUILTIN_MESH("Cube", Utils::Path::GetAbsolute("/Resources/Models/Capsule/untitled.fbx"));
                    }

                    ImGui::EndPopup();
                }
                ImGui::EndPopup();
            }

            ImGui::EndPopup();
        }

        // Add padding around the list
        m_Context->m_Registry.each(
            [this](auto entityId)
            {
                Entity entity{entityId, m_Context.get()};
                DrawEntityNode(entity);
            });
    }
    ImGui::End();

    ImGui::Begin("Properties");
    if (m_SelectionContext)
    {
        if (m_SelectionContext.HasComponent<TagComponent>())
        {
            auto &tag = m_SelectionContext.GetComponent<TagComponent>().Tag;
            char buffer[256];
            memset(buffer, 0, sizeof(buffer));
            strcpy(buffer, tag.c_str());

            ImGui::Text("Tag  ");
            ImGui::SameLine();
            if (ImGui::InputText("##tag", buffer, sizeof(buffer)))
            {
                tag = std::string(buffer);
            }
        }
        ImGui::SameLine();

        float buttonWidth = ImGui::CalcTextSize(ICON_FA_PLUS "   Add Component").x + 20.0f; // Adjust padding as needed
        ImGui::SetCursorPosX(ImGui::GetContentRegionMax().x - buttonWidth);
        if (ImGui::Button(ICON_FA_PLUS "   Add Component", ImVec2(buttonWidth, 0))) ImGui::OpenPopup("AddComponent");

        // TODO: refactor this so you dont have to manually list the components
        if (ImGui::BeginPopup("AddComponent"))
        {
            ADD_COMPONENT_MENU(LuaScriptComponent, ICON_FA_SCROLL "   Lua Script");
            ADD_COMPONENT_MENU(RigidBodyComponent, ICON_FA_WEIGHT_HANGING "   Rigid Body");
            ADD_COMPONENT_MENU(BoxColliderComponent, ICON_FA_CUBE "   Box Collider");

            ImGui::EndPopup();
        }
        ImGui::Spacing();
        ImGui::Spacing();
        DrawComponents(m_SelectionContext);
    }
    ImGui::End();
}
} // namespace Engine