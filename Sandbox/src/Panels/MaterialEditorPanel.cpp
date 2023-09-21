#include "MaterialEditorPanel.h"

#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/glm.hpp>

#include "AssetManager.h"
#include "Material.h"
#include "Log.h"
#include "MaterialSerializer.h"
#include "ImGuiHelpers.h"
#include "FileDialogs.h"

#include <IconsFontAwesome5.h>

namespace Engine
{
AssetHandle MaterialEditorPanel::s_MaterialHandle = 0;
bool MaterialEditorPanel::s_ShowMaterialEditor = false;
bool MaterialEditorPanel::s_IsDefaultMaterial = false;

static void ResetMaterial(MaterialRef material, ParameterType type)
{
    ImGui::SameLine();
    ImGui::SetCursorPosX(ImGui::GetWindowWidth() - 40);
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
    if (ImGui::Button(" " ICON_FA_UNDO " ")) material->Reset(type);
    ImGui::PopStyleColor();
	ImGui::SetCursorPosX(-1);
}

static void ShowTootip(const char* text, Texture2DRef texture)
{
    // tool tip
    if (ImGui::IsItemHovered())
    {
		ImGui::BeginTooltip();

		// draw zoomed in texture
		// header
		ImGui::Text(text);
		ImGui::Separator();
		ImGui::Image((void *)(intptr_t)texture->GetRendererID(), ImVec2(256, 256), ImVec2(0, 1), ImVec2(1, 0));

		ImGui::EndTooltip();
    }
}

MaterialEditorPanel::MaterialEditorPanel()
{
    auto path = "Resources/Textures/Checkerboard.png";
    m_CheckerboardTexture = TextureImporter::LoadTexture2D(path);
}

void MaterialEditorPanel::OnImGuiRender()
{
    if (!s_ShowMaterialEditor) return;

    ImGui::Begin("Material Editor", &s_ShowMaterialEditor);
    auto material = AssetManager::GetAsset<Material>(s_MaterialHandle);

    _collapsingHeaderStyle();
	ImGui::PushID("Material");
    if (ImGui::CollapsingHeader("Albedo", ImGuiTreeNodeFlags_AllowOverlap))
    {
		ResetMaterial(material, ParameterType::ALBEDO);
        auto albedoTexture = material->GetTexture(ParameterType::ALBEDO) ? material->GetTexture(ParameterType::ALBEDO)
                                                                         : m_CheckerboardTexture;

        ImGui::Image((void *)(intptr_t)albedoTexture->GetRendererID(), ImVec2(90, 90), ImVec2(0, 1), ImVec2(1, 0));

		ShowTootip("Albedo", albedoTexture);
        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
            {
                const char *handle = (const char *)payload->Data;
                material->SetTexture(ParameterType::ALBEDO, std::stoull(handle));
            }
            ImGui::EndDragDropTarget();
        }
        if (ImGui::BeginPopupContextItem("material_texture"))
        {
            if (ImGui::MenuItem(ICON_FA_TRASH "   Remove")) material->SetTexture(ParameterType::ALBEDO, 0);
            ImGui::EndPopup();
        }
        ImGui::SameLine();

        glm::vec3 color = material->GetMaterialData().Albedo;
        ImGui::ColorEdit3("Color", glm::value_ptr(color), ImGuiColorEditFlags_NoInputs);
        material->SetMaterialParam(ParameterType::ALBEDO, color);
    }
    ImGui::PopID();

    _collapsingHeaderStyle();
	ImGui::PushID("Normal");
    if (ImGui::CollapsingHeader("Normal", ImGuiTreeNodeFlags_AllowOverlap))
    {
		ResetMaterial(material, ParameterType::NORMAL);
        auto normalTexture = material->GetTexture(ParameterType::NORMAL) ? material->GetTexture(ParameterType::NORMAL)
                                                                         : m_CheckerboardTexture;

        ImGui::Image((void *)(intptr_t)normalTexture->GetRendererID(), ImVec2(90, 90), ImVec2(0, 1), ImVec2(1, 0));

		ShowTootip("Normal", normalTexture);
        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
            {
                const char *handle = (const char *)payload->Data;
                material->SetTexture(ParameterType::NORMAL, std::stoull(handle));
            }
            ImGui::EndDragDropTarget();
        }
        if (ImGui::BeginPopupContextItem("material_texture"))
        {
            if (ImGui::MenuItem(ICON_FA_TRASH "   Remove")) material->SetTexture(ParameterType::NORMAL, 0);
            ImGui::EndPopup();
        }
        ImGui::SameLine();

        bool useNormalMap = material->GetUseNormalMap();
        ImGui::Checkbox("Use", &useNormalMap);
        material->SetUseNormalMap(useNormalMap);
    }
	ImGui::PopID();

    _collapsingHeaderStyle();
    ImGui::PushID("Metallic");
    if (ImGui::CollapsingHeader("Metallic", ImGuiTreeNodeFlags_AllowOverlap))
    {
		ResetMaterial(material, ParameterType::METALLIC);
        auto metallicTexture = material->GetTexture(ParameterType::METALLIC)
                                   ? material->GetTexture(ParameterType::METALLIC)
                                   : m_CheckerboardTexture;

        ImGui::Image((void *)(intptr_t)metallicTexture->GetRendererID(), ImVec2(90, 90), ImVec2(0, 1), ImVec2(1, 0));

		ShowTootip("Metallic", metallicTexture);
        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
            {
                const char *handle = (const char *)payload->Data;
                material->SetTexture(ParameterType::METALLIC, std::stoull(handle));
            }
            ImGui::EndDragDropTarget();
        }
        if (ImGui::BeginPopupContextItem("material_texture"))
        {
            if (ImGui::MenuItem(ICON_FA_TRASH "   Remove")) material->SetTexture(ParameterType::METALLIC, 0);
            ImGui::EndPopup();
        }
        ImGui::SameLine();
        float metallic = material->GetMaterialData().Metallic;
        ImGui::PushItemWidth(200);
        ImGui::SliderFloat("Value", &metallic, 0.0f, 1.0f);
        ImGui::PopItemWidth();
        material->SetMaterialParam(ParameterType::METALLIC, metallic);
    }
    ImGui::PopID();

    _collapsingHeaderStyle();
	ImGui::PushID("Roughness");
    if (ImGui::CollapsingHeader("Roughness", ImGuiTreeNodeFlags_AllowOverlap))
    {
		ResetMaterial(material, ParameterType::ROUGHNESS);
        auto roughnessTexture = material->GetTexture(ParameterType::ROUGHNESS)
                                    ? material->GetTexture(ParameterType::ROUGHNESS)
                                    : m_CheckerboardTexture;

        ImGui::Image((void *)(intptr_t)roughnessTexture->GetRendererID(), ImVec2(90, 90), ImVec2(0, 1), ImVec2(1, 0));

		ShowTootip("Roughness", roughnessTexture);
        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
            {
                const char *handle = (const char *)payload->Data;
                material->SetTexture(ParameterType::ROUGHNESS, std::stoull(handle));
            }
            ImGui::EndDragDropTarget();
        }
        if (ImGui::BeginPopupContextItem("material_texture"))
        {
            if (ImGui::MenuItem(ICON_FA_TRASH "   Remove")) material->SetTexture(ParameterType::ROUGHNESS, 0);
            ImGui::EndPopup();
        }
        ImGui::SameLine();

        float roughness = material->GetMaterialData().Roughness;
        ImGui::PushItemWidth(200);
        ImGui::SliderFloat("Value", &roughness, 0.0f, 1.0f);
        ImGui::PopItemWidth();
        material->SetMaterialParam(ParameterType::ROUGHNESS, roughness);
    }
	ImGui::PopID();

	_collapsingHeaderStyle();
    ImGui::PushID("Ao");
    if (ImGui::CollapsingHeader("Ambient Occlusion", ImGuiTreeNodeFlags_AllowOverlap))
    {
		ResetMaterial(material, ParameterType::AO);
		auto aoTexture = material->GetTexture(ParameterType::AO)
									? material->GetTexture(ParameterType::AO)
									: m_CheckerboardTexture;

		ImGui::Image((void *)(intptr_t)aoTexture->GetRendererID(), ImVec2(90, 90), ImVec2(0, 1), ImVec2(1, 0));

		ShowTootip("Ao", aoTexture);
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
			{
				const char *handle = (const char *)payload->Data;
				material->SetTexture(ParameterType::AO, std::stoull(handle));
			}
			ImGui::EndDragDropTarget();
		}
		if (ImGui::BeginPopupContextItem("material_texture"))
		{
			if (ImGui::MenuItem(ICON_FA_TRASH "   Remove")) material->SetTexture(ParameterType::AO, 0);
			ImGui::EndPopup();
		}
		ImGui::SameLine();

		float ao = material->GetMaterialData().AO;
        ImGui::PushItemWidth(200);
        ImGui::SliderFloat("Value", &ao, 0.0f, 1.0f);
        ImGui::PopItemWidth();
        material->SetMaterialParam(ParameterType::AO, ao);
    }
    ImGui::PopID();

	if (material->IsDefault)
    {
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 20);

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.2f, 0.2f, 0.5f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.3f, 0.3f, 0.6f));
        if (ImGui::Button(ICON_FA_SAVE "   Save as new material"))
        {
            std::string path = FileDialogs::SaveFile("Material (*.material)\0*.material\0");
            if (!path.empty())
            {
                MaterialSerializer serializer(material);
                serializer.Serialize(path);
                s_ShowMaterialEditor = false;
            }
        }
        ImGui::PopStyleColor(2);
    }

    ImGui::End();
}

void MaterialEditorPanel::OpenMaterialEditor(AssetHandle handle, bool isDefaultMaterial)
{
    if (AssetManager::IsAssetHandleValid(handle))
    {
        s_IsDefaultMaterial = isDefaultMaterial;
        s_ShowMaterialEditor = true;
        s_MaterialHandle = handle;
    }
    else
        LOG_CORE_ERROR("MaterialEditorPanel::OpenMaterialEditor - Invalid asset handle!");
}
} // namespace Engine
