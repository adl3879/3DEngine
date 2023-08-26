#include "MaterialEditorPanel.h"

#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/glm.hpp>

#include "AssetManager.h"
#include "Material.h"
#include "Log.h"
#include "PlatformUtils.h"
#include "MaterialSerializer.h"
#include <IconsFontAwesome5.h>

namespace Engine
{
AssetHandle MaterialEditorPanel::s_MaterialHandle = 0;
bool MaterialEditorPanel::s_ShowMaterialEditor = false;

MaterialEditorPanel::MaterialEditorPanel()
{
    auto path = Utils::Path::GetAbsolute("/Resources/Textures/Checkerboard.png");
    m_CheckerboardTexture = TextureImporter::LoadTexture2D(path);
}

void MaterialEditorPanel::OnImGuiRender()
{
    if (!s_ShowMaterialEditor) return;

    ImGui::Begin("Material Editor", &s_ShowMaterialEditor);
    auto material = AssetManager::GetAsset<Material>(s_MaterialHandle);

    if (ImGui::TreeNodeEx("Albedo", ImGuiTreeNodeFlags_DefaultOpen, "Albedo"))
    {
        auto albedoTexture = material->GetTexture(ParameterType::ALBEDO) ? material->GetTexture(ParameterType::ALBEDO)
                                                                         : m_CheckerboardTexture;

        ImGui::Image((void *)(intptr_t)albedoTexture->GetRendererID(), ImVec2(90, 90), ImVec2(0, 1), ImVec2(1, 0));
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

        ImGui::TreePop();
    }
    if (ImGui::TreeNodeEx("Normal", ImGuiTreeNodeFlags_DefaultOpen, "Normal"))
    {
        auto normalTexture = material->GetTexture(ParameterType::NORMAL) ? material->GetTexture(ParameterType::NORMAL)
                                                                         : m_CheckerboardTexture;

        ImGui::Image((void *)(intptr_t)normalTexture->GetRendererID(), ImVec2(90, 90), ImVec2(0, 1), ImVec2(1, 0));
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

        ImGui::TreePop();
    }
    if (ImGui::TreeNodeEx("Metallic", ImGuiTreeNodeFlags_DefaultOpen, "Metallic"))
    {
        auto metallicTexture = material->GetTexture(ParameterType::METALLIC)
                                   ? material->GetTexture(ParameterType::METALLIC)
                                   : m_CheckerboardTexture;

        ImGui::Image((void *)(intptr_t)metallicTexture->GetRendererID(), ImVec2(90, 90), ImVec2(0, 1), ImVec2(1, 0));
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
        ImGui::SliderFloat("Metallic", &metallic, 0.0f, 1.0f);
        ImGui::PopItemWidth();
        material->SetMaterialParam(ParameterType::METALLIC, metallic);

        ImGui::TreePop();
    }
    if (ImGui::TreeNodeEx("Roughness", ImGuiTreeNodeFlags_DefaultOpen, "Roughness"))
    {
        auto roughnessTexture = material->GetTexture(ParameterType::ROUGHNESS)
                                    ? material->GetTexture(ParameterType::ROUGHNESS)
                                    : m_CheckerboardTexture;

        ImGui::Image((void *)(intptr_t)roughnessTexture->GetRendererID(), ImVec2(90, 90), ImVec2(0, 1), ImVec2(1, 0));
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
        ImGui::SliderFloat("Roughness", &roughness, 0.0f, 1.0f);
        ImGui::PopItemWidth();
        material->SetMaterialParam(ParameterType::ROUGHNESS, roughness);

        ImGui::TreePop();
    }

    if (ImGui::Button("Save and Quit"))
    {
        auto mat = AssetManager::GetRegistry()[s_MaterialHandle];
        MaterialSerializer serializer(material);
        serializer.Serialize(Project::GetAssetDirectory() / mat.FilePath);
        s_ShowMaterialEditor = false;
    }

    ImGui::End();
}

void MaterialEditorPanel::OpenMaterialEditor(AssetHandle handle)
{
    s_ShowMaterialEditor = true;
    s_MaterialHandle = handle;
}
} // namespace Engine