#include "EnvironmentPanel.h"

#include <imgui.h>
#include "ImGuiHelpers.h"

namespace Engine
{
EnvironmentPanel::EnvironmentPanel() {}

void EnvironmentPanel::OnImGuiRender()
{
    auto environment = m_Context->GetEnvironment();
    ImGui::Begin("Environment");
    ImGui::BeginGroup();
    ImGui::Combo(_labelPrefix("Sky Type").c_str(), (int *)&environment->CurrentSkyType,
                 "Clear Color\0Procedural Sky\0Skybox HDR\0");

    ImGui::Spacing();

    if (environment->CurrentSkyType == SkyType::ClearColor)
    {
        ImGui::ColorEdit3(_labelPrefix("Color", "Environment").c_str(), glm::value_ptr(environment->AmbientColor));
    }

    if (environment->CurrentSkyType == SkyType::SkyboxHDR)
    {
        // auto &component = entity.GetComponent<SkyLightComponent>();
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.05f, 0.05f, 0.05f, 0.54f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.05f, 0.05f, 0.05f, 0.54f));
        ImGui::Button(_labelPrefix("Texture", "Environment").c_str(), ImVec2(ImGui::GetContentRegionAvail().x, 0.0f));
        ImGui::PopStyleColor(2);
        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
            {
                const char *handleStr = (const char *)payload->Data;
                AssetHandle handle = std::stoull(handleStr);
                // component.Use(handle, 2048);
                m_Context->GetEnvironment()->SkyboxHDR = std::make_shared<SkyLight>();
                m_Context->GetEnvironment()->SkyboxHDR->Init(handle, 2048);
            }
            ImGui::EndDragDropTarget();
        }
    }
    ImGui::EndGroup();
    ImGui::End();
}
} // namespace Engine