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
#include "Misc/ThumbnailManager.h"

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
    if (ImGui::Button(" " ICON_FA_UNDO " "))
    {
        material->Reset(type);
        ThumbnailManager::Get().MarkThumbnailAsDirty(material->Handle);
    }
    ImGui::PopStyleColor();
    ImGui::SetCursorPosX(-1);
}

static void ShowTooltip(const char *text, Texture2DRef texture)
{
    // tool tip
    if (ImGui::IsItemHovered())
    {
        ImGui::BeginTooltip();

        // draw zoomed in texture
        // header
        ImGui::TextWrapped("%s", text);
        ImGui::Separator();
        ImGui::Image((void *)(intptr_t)texture->GetRendererID(), ImVec2(256, 256), ImVec2(0, 1), ImVec2(1, 0));

        ImGui::EndTooltip();
    }
}

MaterialEditorPanel::MaterialEditorPanel()
{
    const auto path = "Resources/Textures/Checkerboard.png";
    m_CheckerboardTexture = TextureImporter::LoadTexture2D(path);
}

void MaterialEditorPanel::OnImGuiRender()
{
    if (!s_ShowMaterialEditor) return;

    ImGui::Begin("Material Editor", &s_ShowMaterialEditor);
    auto material = AssetManager::GetAsset<Material>(s_MaterialHandle);

    auto path = AssetManager::GetRegistry()[s_MaterialHandle].FilePath;
    int thumbnailSize = 256;

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.2f, 0.2f, 0.0f));
    ImGui::Button(
        _labelPrefix("Name", !material->Name.empty() ? material->Name.c_str() : path.stem().string().c_str()));
    ImGui::Button(_labelPrefix("Handle", std::to_string(s_MaterialHandle).c_str()));
    ImGui::PopStyleColor();

    // shader
    ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.0f, 0.5f));
    ImGui::Button(_labelPrefix("Shader", AssetManager::GetAssetName(material->GetShaderHandle()).c_str()),
                  ImVec2(-1, 0));
    ImGui::PopStyleVar();

    if (ImGui::BeginDragDropTarget())
    {
        if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
        {
            const char *p = (const char *)payload->Data;
            if (Utils::GetAssetTypeFromExtension(p) == AssetType::Shader)
            {
                material->SetShaderHandle(AssetManager::GetAssetHandleFromPath(p));
            }
        }
        ImGui::EndDragDropTarget();
    }

    _collapsingHeaderStyle();
    ImGui::PushID("Preview");
    if (ImGui::CollapsingHeader("Preview", ImGuiTreeNodeFlags_AllowOverlap))
    {
        // align to center
        ImGui::SetCursorPosX((ImGui::GetWindowWidth() - thumbnailSize) / 2);
        const auto icon = ThumbnailManager::Get()
            .GetThumbnail(AssetManager::GetRegistry()[s_MaterialHandle].FilePath);
        if (icon)
        {
            ImGui::Image((void *)(intptr_t)icon->GetRendererID(),
                         ImVec2(thumbnailSize, thumbnailSize), ImVec2(0, 1), ImVec2(1, 0));
        }
    }
    ImGui::PopID();

    _collapsingHeaderStyle(true);
    ImGui::PushID("Material");
    if (ImGui::CollapsingHeader("Albedo", ImGuiTreeNodeFlags_AllowOverlap))
    {
        ResetMaterial(material, ParameterType::ALBEDO);
        auto albedoTexture = material->GetTexture(ParameterType::ALBEDO)
                                 ? material->GetTexture(ParameterType::ALBEDO)
                                 : m_CheckerboardTexture;

        ImGui::Image((void *)(intptr_t)albedoTexture->GetRendererID(), ImVec2(90, 90), ImVec2(0, 1), ImVec2(1, 0));

        ShowTooltip(material->GetTexturePath(ParameterType::ALBEDO).c_str(), albedoTexture);
        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
            {
                const char *path = (const char *)payload->Data;
                material->SetTexture(ParameterType::ALBEDO, AssetManager::GetAssetHandleFromPath(path));
                ThumbnailManager::Get().MarkThumbnailAsDirty(s_MaterialHandle);
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
        if (ImGui::ColorEdit3("Color", glm::value_ptr(color), ImGuiColorEditFlags_NoInputs))
        {
            material->SetMaterialParam(ParameterType::ALBEDO, color);
            ThumbnailManager::Get().MarkThumbnailAsDirty(s_MaterialHandle);
        }

        ImGui::SameLine();
        float emissiveValue = material->GetMaterialData().Emissive;
        ImGui::PushItemWidth(200);
        ImGui::DragFloat("Emissive", &emissiveValue, 0.5, 1);
        material->SetEmissiveValue(emissiveValue);
    }
    ImGui::PopID();

    _collapsingHeaderStyle(true);
    ImGui::PushID("Normal");
    if (ImGui::CollapsingHeader("Normal", ImGuiTreeNodeFlags_AllowOverlap))
    {
        ResetMaterial(material, ParameterType::NORMAL);
        auto normalTexture = material->GetTexture(ParameterType::NORMAL)
                                 ? material->GetTexture(ParameterType::NORMAL)
                                 : m_CheckerboardTexture;

        ImGui::Image((void *)(intptr_t)normalTexture->GetRendererID(), ImVec2(90, 90), ImVec2(0, 1), ImVec2(1, 0));

        ShowTooltip(material->GetTexturePath(ParameterType::NORMAL).c_str(), normalTexture);
        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
            {
                const char *path = (const char *)payload->Data;
                material->SetTexture(ParameterType::NORMAL, AssetManager::GetAssetHandleFromPath(path));
                ThumbnailManager::Get().MarkThumbnailAsDirty(s_MaterialHandle);
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
        if (ImGui::Checkbox("Use", &useNormalMap))
        {
            ThumbnailManager::Get().MarkThumbnailAsDirty(s_MaterialHandle);
        }
        material->SetUseNormalMap(useNormalMap);
    }
    ImGui::PopID();

    _collapsingHeaderStyle(true);
    ImGui::PushID("Metallic");
    if (ImGui::CollapsingHeader("Metallic", ImGuiTreeNodeFlags_AllowOverlap))
    {
        ResetMaterial(material, ParameterType::METALLIC);
        auto metallicTexture = material->GetTexture(ParameterType::METALLIC)
                                   ? material->GetTexture(ParameterType::METALLIC)
                                   : m_CheckerboardTexture;

        ImGui::Image((void *)(intptr_t)metallicTexture->GetRendererID(), ImVec2(90, 90), ImVec2(0, 1), ImVec2(1, 0));

        ShowTooltip(material->GetTexturePath(ParameterType::METALLIC).c_str(), metallicTexture);
        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
            {
                const char *path = (const char *)payload->Data;
                material->SetTexture(ParameterType::METALLIC, AssetManager::GetAssetHandleFromPath(path));
                ThumbnailManager::Get().MarkThumbnailAsDirty(s_MaterialHandle);
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
        if (ImGui::SliderFloat("Value", &metallic, 0.0f, 1.0f))
        {
            ThumbnailManager::Get().MarkThumbnailAsDirty(s_MaterialHandle);
        }
        ImGui::PopItemWidth();
        material->SetMaterialParam(ParameterType::METALLIC, metallic);
    }
    ImGui::PopID();

    _collapsingHeaderStyle(true);
    ImGui::PushID("Roughness");
    if (ImGui::CollapsingHeader("Roughness", ImGuiTreeNodeFlags_AllowOverlap))
    {
        ResetMaterial(material, ParameterType::ROUGHNESS);
        auto roughnessTexture = material->GetTexture(ParameterType::ROUGHNESS)
                                    ? material->GetTexture(ParameterType::ROUGHNESS)
                                    : m_CheckerboardTexture;

        ImGui::Image((void *)(intptr_t)roughnessTexture->GetRendererID(), ImVec2(90, 90), ImVec2(0, 1), ImVec2(1, 0));

        ShowTooltip(material->GetTexturePath(ParameterType::ROUGHNESS).c_str(), roughnessTexture);
        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
            {
                const char *path = (const char *)payload->Data;
                material->SetTexture(ParameterType::ROUGHNESS, AssetManager::GetAssetHandleFromPath(path));
                ThumbnailManager::Get().MarkThumbnailAsDirty(s_MaterialHandle);
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
        if (ImGui::SliderFloat("Value", &roughness, 0.0f, 1.0f))
        {
            ThumbnailManager::Get().MarkThumbnailAsDirty(s_MaterialHandle);
        }
        ImGui::PopItemWidth();
        material->SetMaterialParam(ParameterType::ROUGHNESS, roughness);
    }
    ImGui::PopID();

    _collapsingHeaderStyle(true);
    ImGui::PushID("Ao");
    if (ImGui::CollapsingHeader("Ambient Occlusion", ImGuiTreeNodeFlags_AllowOverlap))
    {
        ResetMaterial(material, ParameterType::AO);
        auto aoTexture =
            material->GetTexture(ParameterType::AO) ? material->GetTexture(ParameterType::AO) : m_CheckerboardTexture;

        ImGui::Image((void *)(intptr_t)aoTexture->GetRendererID(), ImVec2(90, 90), ImVec2(0, 1), ImVec2(1, 0));

        ShowTooltip(material->GetTexturePath(ParameterType::AO).c_str(), aoTexture);
        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
            {
                const char *path = (const char *)payload->Data;
                material->SetTexture(ParameterType::AO, AssetManager::GetAssetHandleFromPath(path));
                ThumbnailManager::Get().MarkThumbnailAsDirty(s_MaterialHandle);
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
        if (ImGui::SliderFloat("Value", &ao, 0.0f, 1.0f))
        {
            ThumbnailManager::Get().MarkThumbnailAsDirty(s_MaterialHandle);
        }
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
