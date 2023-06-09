#include "SceneHierarchyPanel.h"

#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/glm.hpp>

#include "Light.h"

namespace Engine
{
SceneHierarchyPanel::SceneHierarchyPanel(const std::shared_ptr<Scene> &context)
{
    //
    SetContext(context);
}

void SceneHierarchyPanel::DrawEntityNode(Entity entity)
{
    auto &tag = entity.GetComponent<TagComponent>().Tag;

    auto flags = ((m_SelectionContext == entity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
    bool opened = ImGui::TreeNodeEx((void *)(uint64_t)(uint32_t)entity, flags, "%s", tag.c_str());
    if (ImGui::IsItemClicked()) m_SelectionContext = entity;

    bool entityDeleted = false;
    if (ImGui::BeginPopupContextItem())
    {
        if (ImGui::MenuItem("Delete Entity")) entityDeleted = true;

        ImGui::EndPopup();
    }

    if (opened) ImGui::TreePop();

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
    if (entity.HasComponent<TagComponent>())
    {
        auto &tag = entity.GetComponent<TagComponent>().Tag;
        char buffer[256];
        memset(buffer, 0, sizeof(buffer));
        strcpy(buffer, tag.c_str());

        if (ImGui::InputText("Tag", buffer, sizeof(buffer)))
        {
            tag = std::string(buffer);
        }
    }

    if (entity.HasComponent<TransformComponent>())
    {
        if (ImGui::TreeNodeEx((void *)typeid(TransformComponent).hash_code(), ImGuiTreeNodeFlags_DefaultOpen,
                              "Transform"))
        {
            auto &translation = entity.GetComponent<TransformComponent>().Translation;
            ImGui::DragFloat3("Position", glm::value_ptr(translation), 0.1f);

            auto &rotation = entity.GetComponent<TransformComponent>().Rotation;
            ImGui::DragFloat3("Rotation", glm::value_ptr(rotation), 0.1f);

            auto &scale = entity.GetComponent<TransformComponent>().Scale;
            ImGui::DragFloat3("Scale", glm::value_ptr(scale), 0.1f);

            ImGui::TreePop();
        }
    }

    if (entity.HasComponent<CameraComponent>())
    {
        bool removeComponent = false;
        // if (ImGui::BeginPopupContextItem())
        // {
        //     if (ImGui::MenuItem("Remove Component"))
        //         removeComponent = true;

        //     ImGui::EndPopup();
        // }

        if (ImGui::TreeNodeEx((void *)typeid(CameraComponent).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, "Camera"))
        {
            auto &cameraComponent = entity.GetComponent<CameraComponent>();
            auto &camera = cameraComponent.Camera;

            if (ImGui::Checkbox("Primary", &cameraComponent.Primary))
            {
                auto view = m_Context->m_Registry.view<CameraComponent>();
                for (auto entity : view)
                {
                    auto &otherCameraComponent = view.get<CameraComponent>(entity);
                    if (&otherCameraComponent != &cameraComponent) otherCameraComponent.Primary = false;
                }
            }

            ImGui::Separator();

            float perspectiveVerticalFOV = glm::degrees(camera.GetPerspectiveVerticalFOV());
            if (ImGui::DragFloat("Vertical FOV", &perspectiveVerticalFOV))
                camera.SetPerspectiveVerticalFOV(glm::radians(perspectiveVerticalFOV));

            float perspectiveNearClip = camera.GetPerspectiveNearClip();
            if (ImGui::DragFloat("Near Clip", &perspectiveNearClip)) camera.SetPerspectiveNearClip(perspectiveNearClip);

            float perspectiveFarClip = camera.GetPerspectiveFarClip();
            if (ImGui::DragFloat("Far Clip", &perspectiveFarClip)) camera.SetPerspectiveFarClip(perspectiveFarClip);

            ImGui::TreePop();
        }

        if (removeComponent) entity.RemoveComponent<CameraComponent>();
    }

    if (entity.HasComponent<ModelComponent>())
    {
        bool removeComponent = false;
        // if (ImGui::BeginPopupContextItem())
        // {
        //     if (ImGui::MenuItem("Remove Component"))
        //         removeComponent = true;

        //     ImGui::EndPopup();
        // }
        auto &entityComponent = entity.GetComponent<ModelComponent>();
        if (ImGui::TreeNodeEx((void *)typeid(ModelComponent).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, "Model"))
        {
            char buffer[256];
            memset(buffer, 0, sizeof(buffer));
            strcpy(buffer, entityComponent.Path.c_str());
            if (ImGui::InputText("Path", buffer, sizeof(buffer))) entityComponent.Path = std::string(buffer);
            ImGui::SameLine();
            if (ImGui::Button("..."))
            {
                Utils::FileDialogs::OpenFile(
                    "modelOpen",
                    Utils::FileDialogParams{
                        .DefaultPathAndFile = "/home/adeleye/Source/3DEngine/src/Sandbox/res/models/",
                        .SingleFilterDescription = "Model (*.fbx;*.obj;*.dae;*.gltf)\0*.fbx;*.obj;*.dae;*.gltf\0",
                    });
            }

            if (Utils::FileDialogs::FileIsOpened("modelOpen"))
            {
                auto path = Utils::Path::GetRelative(Utils::FileDialogs::m_SelectedFile);
                entityComponent.Path = path;
                entityComponent.Create(path.c_str());
            }

            ImGui::TreePop();
        }

        if (removeComponent) entity.RemoveComponent<ModelComponent>();
    }

    if (entity.HasComponent<DirectionalLightComponent>())
    {
        if (ImGui::TreeNodeEx((void *)typeid(DirectionalLightComponent).hash_code(), ImGuiTreeNodeFlags_DefaultOpen,
                              "DirectionalLight"))
        {
            auto &entityComponent = entity.GetComponent<DirectionalLightComponent>();
            ImGui::DragFloat3("Direction", glm::value_ptr(entityComponent.Light.Direction), 1.0f, 0.0f, 1.0f);
            ImGui::ColorEdit3("Color", glm::value_ptr(entityComponent.Light.Color));
            ImGui::DragFloat("Ambient Intensity", &entityComponent.Light.AmbientIntensity, 0.001f, 0.0f, 1.0f);
            ImGui::DragFloat("Diffuse Intensity", &entityComponent.Light.DiffuseIntensity, 0.001f, 0.0f, 1.0f);

            Light::SetDirectionalLight(&entityComponent.Light);

            ImGui::TreePop();
        }
    }

    if (entity.HasComponent<PointLightComponent>())
    {
        if (ImGui::TreeNodeEx((void *)typeid(PointLightComponent).hash_code(), ImGuiTreeNodeFlags_DefaultOpen,
                              "PointLight"))
        {
            auto &entityComponent = entity.GetComponent<PointLightComponent>();
            auto &transform = entity.GetComponent<TransformComponent>();
            entityComponent.Light.Position = transform.Translation;
            ImGui::ColorEdit3("Color", glm::value_ptr(entityComponent.Light.Color));
            ImGui::DragFloat("Ambience", &entityComponent.Light.AmbientIntensity, 0.001f, 0.0f, 1.0f);
            ImGui::DragFloat("Diffusion", &entityComponent.Light.DiffuseIntensity, 0.001f, 0.0f, 1.0f);
            ImGui::DragFloat("Constant", &entityComponent.Light.Attenuation.Constant, 0.001f, 0.0f, 1.0f);
            ImGui::DragFloat("Linear", &entityComponent.Light.Attenuation.Linear, 0.001f, 0.0f, 1.0f);
            ImGui::DragFloat("Exponential", &entityComponent.Light.Attenuation.Exp, 0.001f, 0.0f, 1.0f);

            Light::SetPointLight(entityComponent.Light, entityComponent.Index);

            ImGui::TreePop();
        }
    }

    if (entity.HasComponent<SpotLightComponent>())
    {
        if (ImGui::TreeNodeEx((void *)typeid(SpotLightComponent).hash_code(), ImGuiTreeNodeFlags_DefaultOpen,
                              "SpotLight"))
        {
            auto &entityComponent = entity.GetComponent<SpotLightComponent>();
            auto &transform = entity.GetComponent<TransformComponent>();
            entityComponent.Light.Position = transform.Translation;
            entityComponent.Light.Direction = transform.Rotation;
            ImGui::ColorEdit3("Color", glm::value_ptr(entityComponent.Light.Color));
            ImGui::DragFloat("Ambient Intensity", &entityComponent.Light.AmbientIntensity, 0.001f, 0.0f, 1.0f);
            ImGui::DragFloat("Diffuse Intensity", &entityComponent.Light.DiffuseIntensity, 0.001f, 0.0f, 1.0f);
            ImGui::DragFloat("Constant", &entityComponent.Light.Attenuation.Constant, 0.001f, 0.0f, 1.0f);
            ImGui::DragFloat("Linear", &entityComponent.Light.Attenuation.Linear, 0.001f, 0.0f, 1.0f);
            ImGui::DragFloat("Exponential", &entityComponent.Light.Attenuation.Exp, 0.001f, 0.0f, 1.0f);
            ImGui::DragFloat("Cutoff", &entityComponent.Light.Cutoff, 0.1f, 0.0f, 90.0f);
            ImGui::DragFloat("Outer Cutoff", &entityComponent.Light.OuterCutoff, 0.1f, 0.0f, 90.0f);

            Light::SetSpotLight(entityComponent.Light, entityComponent.Index);

            ImGui::TreePop();
        }
    }
}

void SceneHierarchyPanel::SetContext(const std::shared_ptr<Scene> &context)
{
    m_Context = context;
    m_SelectionContext = {};
}

void SceneHierarchyPanel::OnImGuiRender()
{
    ImGui::Begin("Scene Hierarchy");

    if (ImGui::Button("ADD")) ImGui::OpenPopup("my_file_popup");
    if (ImGui::BeginPopup("my_file_popup", ImGuiWindowFlags_MenuBar))
    {
        if (ImGui::BeginMenuBar())
        {
            ImGui::EndMenuBar();
        }

        // search bar
        static char buf[64] = "";
        ImGui::InputTextWithHint("##Search", "Search", buf, IM_ARRAYSIZE(buf));
        ImGui::Separator();
        ImGui::Spacing();

        if (ImGui::MenuItem("Create Empty Entity"))
        {
            auto entity = m_Context->CreateEntity();
            m_SelectionContext = entity;
        }
        ImGui::Spacing();
        ImGui::Separator();

        if (ImGui::MenuItem("Camera"))
        {
            auto entity = m_Context->CreateEntity("Camera");

            entity.AddComponent<CameraComponent>();
            m_SelectionContext = entity;
        }
        ImGui::Spacing();
        ImGui::Separator();

        // check if scene does not contain directional light entity
        auto dLight = m_Context->GetEntity("Directional Light");
        if (dLight == nullptr)
        {
            if (ImGui::MenuItem("Directional Light"))
            {
                auto entity = m_Context->CreateEntity("Directional Light");

                entity.AddComponent<DirectionalLightComponent>();
                entity.RemoveComponent<TransformComponent>();
                m_SelectionContext = entity;
            }
        }
        if (ImGui::MenuItem("Point Light"))
        {
            auto num = Light::GetNumPointLights() + 1;
            auto entity = m_Context->CreateEntity("Point Light " + std::to_string(num));

            entity.AddComponent<PointLightComponent>();
            m_SelectionContext = entity;
        }
        if (ImGui::MenuItem("Spot Light"))
        {
            auto num = Light::GetNumSpotLights() + 1;
            auto entity = m_Context->CreateEntity("Spot Light " + std::to_string(num));

            entity.AddComponent<SpotLightComponent>();
            m_SelectionContext = entity;
        }
        ImGui::Spacing();
        ImGui::Separator();

        if (ImGui::MenuItem("Mesh"))
        {
            auto entity = m_Context->CreateEntity("Mesh");

            entity.AddComponent<ModelComponent>();
            m_SelectionContext = entity;
        }

        ImGui::EndPopup();
    }

    ImGui::Spacing();

    m_Context->m_Registry.each(
        [this](auto entityId)
        {
            Entity entity{entityId, m_Context.get()};
            DrawEntityNode(entity);
        });
    ImGui::End();

    ImGui::Begin("Properties");
    if (m_SelectionContext)
    {
        DrawComponents(m_SelectionContext);

        if (ImGui::Button("Add Component")) ImGui::OpenPopup("AddComponent");

        if (ImGui::BeginPopup("AddComponent"))
        {
            if (!m_SelectionContext.HasComponent<LuaScriptComponent>())
            {
                // TODO: create lua file and bind
                if (ImGui::MenuItem("Lua Script"))
                {
                    m_SelectionContext.AddComponent<LuaScriptComponent>();
                    ImGui::CloseCurrentPopup();
                }
            }
            ImGui::EndPopup();
        }
    }
    ImGui::End();
}
} // namespace Engine