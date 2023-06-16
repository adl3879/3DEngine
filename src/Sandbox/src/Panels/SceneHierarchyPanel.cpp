#include "SceneHierarchyPanel.h"

#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/glm.hpp>

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
    if (ImGui::IsItemClicked())
        m_SelectionContext = entity;

    bool entityDeleted = false;
    if (ImGui::BeginPopupContextItem())
    {
        if (ImGui::MenuItem("Delete Entity"))
            entityDeleted = true;

        ImGui::EndPopup();
    }

    if (opened)
        ImGui::TreePop();

    if (entityDeleted)
    {
        m_Context->DestroyEntity(entity);
        if (m_SelectionContext == entity)
            m_SelectionContext = {};
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

            const char *projectionTypeStrings[] = {"Perspective", "Orthographic"};
            const char *currentProjectionTypeString =
                projectionTypeStrings[(int)cameraComponent.Camera.GetProjectionType()];
            if (ImGui::BeginCombo("Projection", currentProjectionTypeString))
            {
                for (int i = 0; i < 2; i++)
                {
                    bool isSelected = currentProjectionTypeString == projectionTypeStrings[i];
                    if (ImGui::Selectable(projectionTypeStrings[i], isSelected))
                    {
                        currentProjectionTypeString = projectionTypeStrings[i];
                        cameraComponent.Camera.SetProjectionType((Camera::ProjectionType)i);
                    }

                    if (isSelected)
                        ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }

            ImGui::Separator();

            if (ImGui::Checkbox("Primary", &cameraComponent.Primary))
            {
                auto view = m_Context->m_Registry.view<CameraComponent>();
                for (auto entity : view)
                {
                    auto &otherCameraComponent = view.get<CameraComponent>(entity);
                    if (&otherCameraComponent != &cameraComponent)
                        otherCameraComponent.Primary = false;
                }
            }

            ImGui::Separator();

            if (camera.GetProjectionType() == Camera::ProjectionType::Perspective)
            {
                float perspectiveVerticalFOV = glm::degrees(camera.GetPerspectiveVerticalFOV());
                if (ImGui::DragFloat("Vertical FOV", &perspectiveVerticalFOV))
                    camera.SetPerspectiveVerticalFOV(glm::radians(perspectiveVerticalFOV));

                float perspectiveNearClip = camera.GetPerspectiveNearClip();
                if (ImGui::DragFloat("Near Clip", &perspectiveNearClip))
                    camera.SetPerspectiveNearClip(perspectiveNearClip);

                float perspectiveFarClip = camera.GetPerspectiveFarClip();
                if (ImGui::DragFloat("Far Clip", &perspectiveFarClip))
                    camera.SetPerspectiveFarClip(perspectiveFarClip);
            }

            if (camera.GetProjectionType() == Camera::ProjectionType::Orthographic)
            {
                float orthographicSize = camera.GetOrthographicSize();
                if (ImGui::DragFloat("Size", &orthographicSize))
                    camera.SetOrthographicSize(orthographicSize);

                float orthographicNearClip = camera.GetOrthographicNearClip();
                if (ImGui::DragFloat("Near Clip", &orthographicNearClip))
                    camera.SetOrthographicNearClip(orthographicNearClip);

                float orthographicFarClip = camera.GetOrthographicFarClip();
                if (ImGui::DragFloat("Far Clip", &orthographicFarClip))
                    camera.SetOrthographicFarClip(orthographicFarClip);
            }

            ImGui::TreePop();
        }

        if (removeComponent)
            entity.RemoveComponent<CameraComponent>();
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
        if (ImGui::TreeNodeEx((void *)typeid(ModelComponent).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, "Model"))
        {
            auto &entityComponent = entity.GetComponent<ModelComponent>();
            char buffer[256];
            memset(buffer, 0, sizeof(buffer));
            strcpy(buffer, entityComponent.Path.c_str());
            if (ImGui::InputText("Path", buffer, sizeof(buffer)))
                entityComponent.Path = std::string(buffer);

            ImGui::TreePop();
        }

        if (removeComponent)
            entity.RemoveComponent<ModelComponent>();
    }
}

void SceneHierarchyPanel::SetContext(const std::shared_ptr<Scene> &context) { m_Context = context; }

void SceneHierarchyPanel::OnImGuiRender()
{
    ImGui::Begin("Scene Hierarchy");

    m_Context->m_Registry.each(
        [this](auto entityId)
        {
            Entity entity{entityId, m_Context.get()};
            DrawEntityNode(entity);
        });

    if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
        m_SelectionContext = {};

    if (ImGui::BeginPopupContextWindow("", ImGuiPopupFlags_NoOpenOverItems | ImGuiPopupFlags_MouseButtonRight))
    {
        if (ImGui::MenuItem("Create Empty Entity"))
        {
            auto entity = m_Context->CreateEntity();
            m_SelectionContext = entity;
        }
        ImGui::EndPopup();
    }

    ImGui::End();

    ImGui::Begin("Properties");

    if (m_SelectionContext)
    {
        DrawComponents(m_SelectionContext);

        if (ImGui::Button("Add Component"))
            ImGui::OpenPopup("AddComponent");

        if (ImGui::BeginPopup("AddComponent"))
        {
            if (!m_SelectionContext.HasComponent<CameraComponent>())
                if (ImGui::MenuItem("Camera"))
                {
                    m_SelectionContext.AddComponent<CameraComponent>();
                    ImGui::CloseCurrentPopup();
                }

            if (!m_SelectionContext.HasComponent<ModelComponent>())
                if (ImGui::MenuItem("3D Mesh"))
                {
                    m_SelectionContext.AddComponent<ModelComponent>();
                    ImGui::CloseCurrentPopup();
                }
            ImGui::EndPopup();
        }
    }

    ImGui::End();

    ImGui::Begin("Editor Camera");

    auto cameraPos = m_Context->m_EditorCamera->GetPosition();
    if (ImGui::DragFloat3("Position", glm::value_ptr(cameraPos), 0.1f))
        m_Context->m_EditorCamera->SetPosition(cameraPos);
    auto cameraRotation = m_Context->m_EditorCamera->GetRotation();
    if (ImGui::DragFloat3("Rotation", glm::value_ptr(cameraRotation), 0.1f))
        m_Context->m_EditorCamera->SetRotation(cameraRotation);

    ImGui::End();
}
} // namespace Engine