#include "EnvironmentPanel.h"

#include <imgui.h>
#include "ImGuiHelpers.h"
#include "AssetManager.h"

namespace Engine
{
EnvironmentPanel::EnvironmentPanel() {}

void EnvironmentPanel::OnImGuiRender()
{
    auto environment = m_Context->GetEnvironment();
    ImGui::Begin("Environment");
    _collapsingHeaderStyle();
    if (ImGui::CollapsingHeader("Sky"))
    {
        ImGui::Combo(_labelPrefix("Sky Type").c_str(), (int *)&environment->CurrentSkyType,
                     "Clear Color\0Procedural Sky\0Skybox HDR\0");

        ImGui::Spacing();

        if (environment->CurrentSkyType == SkyType::ClearColor)
        {
            ImGui::ColorEdit3(_labelPrefix("Color").c_str(), glm::value_ptr(environment->AmbientColor));
        }

        if (environment->CurrentSkyType == SkyType::SkyboxHDR && environment->SkyboxHDR)
        {
            auto skyName = AssetManager::GetAssetName(environment->SkyboxHDR->GetHandle());
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.05f, 0.05f, 0.05f, 0.54f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.05f, 0.05f, 0.05f, 0.54f));
            ImGui::Button(_labelPrefix("Texture", skyName.c_str()).c_str(),
                          ImVec2(ImGui::GetContentRegionAvail().x, 0.0f));
            ImGui::PopStyleColor(2);
            if (ImGui::BeginDragDropTarget())
            {
                if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
                {
                    const char *handleStr = (const char *)payload->Data;
                    AssetHandle handle = std::stoull(handleStr);
                    m_Context->GetEnvironment()->SkyboxHDR = std::make_shared<SkyLight>();
                    m_Context->GetEnvironment()->SkyboxHDR->Init(handle, 2048);
                }
                ImGui::EndDragDropTarget();
            }
        }

        if (environment->CurrentSkyType == SkyType::ProceduralSky)
        {
            ImGui::DragFloat(_labelPrefix("Sun Intensity").c_str(), &environment->ProceduralSkybox->SunIntensity, 0.01f,
                             0.0f, 0.0f, "%.2f");

            _drawVec3Control("Sun Direction", environment->ProceduralSkybox->SunDirection);

            ImGui::DragFloat(_labelPrefix("Surface Radius").c_str(), &environment->ProceduralSkybox->SurfaceRadius,
                             0.01f, 0.0f, 0.0f, "%.2f");
            ImGui::DragFloat(_labelPrefix("Atmosphere Radius").c_str(),
                             &environment->ProceduralSkybox->AtmosphereRadius, 0.01f, 0.0f, 0.0f, "%.2f");
            _drawVec3Control("Center Point", environment->ProceduralSkybox->CenterPoint);
            _drawVec3Control("Rayleigh Scattering", environment->ProceduralSkybox->RayleighScattering);
            _drawVec3Control("Mie Scattering", environment->ProceduralSkybox->MieScattering);
        }
    }
    ImGui::End();
}
} // namespace Engine