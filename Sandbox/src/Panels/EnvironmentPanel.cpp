#include "EnvironmentPanel.h"

#include <imgui.h>
#include "ImGuiHelpers.h"
#include "AssetManager.h"
#include "Log.h"

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
        ImGui::Combo(_labelPrefix("Sky Type"), (int *)&environment->CurrentSkyType,
                     "Clear Color\0Procedural Sky\0Skybox HDR\0");

        ImGui::Spacing();

        if (environment->CurrentSkyType == SkyType::ClearColor)
        {
            ImGui::ColorEdit3(_labelPrefix("Color"), glm::value_ptr(environment->AmbientColor));
        }

        if (environment->CurrentSkyType == SkyType::SkyboxHDR)
        {
            std::string skyName;
            skyName = environment->SkyboxHDR ? environment->SkyboxHDR->GetHdrPath().stem().string()
                          : "No Skybox HDR";
            
			ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.0f, 0.5f));
            ImGui::Button(_labelPrefix("HDRI Texture", skyName.c_str()), ImVec2(-1, 0.0f));
			ImGui::PopStyleVar();

            if (ImGui::BeginDragDropTarget())
            {
                if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
                {
                    const char *hdrPath = (const char *)payload->Data;
                    if (Utils::GetAssetTypeFromExtension(hdrPath) == AssetType::SkyLight)
                    {
                        m_Context->GetEnvironment()->SkyboxHDR = AssetManager::GetAsset<SkyLight>(hdrPath);
                    }
                }
                ImGui::EndDragDropTarget();
            }
        }

        if (environment->CurrentSkyType == SkyType::ProceduralSky)
        {
            ImGui::DragFloat(_labelPrefix("Sun Intensity"), &environment->ProceduralSkybox->SunIntensity, 0.01f,
                             0.0f, 0.0f, "%.2f");

            _drawVec3Control("Sun Direction", environment->ProceduralSkybox->SunDirection);

            ImGui::DragFloat(_labelPrefix("Surface Radius"), &environment->ProceduralSkybox->SurfaceRadius,
                             0.01f, 0.0f, 0.0f, "%.2f");
            ImGui::DragFloat(_labelPrefix("Atmosphere Radius"),
                             &environment->ProceduralSkybox->AtmosphereRadius, 0.01f, 0.0f, 0.0f, "%.2f");
            _drawVec3Control("Center Point", environment->ProceduralSkybox->CenterPoint);
            _drawVec3Control("Rayleigh Scattering", environment->ProceduralSkybox->RayleighScattering);
            _drawVec3Control("Mie Scattering", environment->ProceduralSkybox->MieScattering);
        }
    }

	_collapsingHeaderStyle();
	if (ImGui::CollapsingHeader("Bloom"))
	{
		ImGui::Checkbox(_labelPrefix("Enabled"), &environment->BloomEnabled);
		ImGui::DragFloat(_labelPrefix("Intensity"), &environment->BloomIntensity, 0.01f, 0.0f, 0.0f, "%.2f");
		ImGui::DragFloat(_labelPrefix("Exposure"), &environment->Exposure, 0.01f, 0.0f, 0.0f, "%.2f");
	}
    ImGui::End();
}
} // namespace Engine
