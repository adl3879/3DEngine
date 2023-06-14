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
    if (opened)
        ImGui::TreePop();
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

                // ImGui::Checkbox("Fixed Aspect Ratio", &cameraComponent.FixedAspectRatio);
            }

            ImGui::TreePop();
        }
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

    ImGui::End();

    ImGui::Begin("Properties");

    if (m_SelectionContext)
        DrawComponents(m_SelectionContext);

    ImGui::End();
}
} // namespace Engine