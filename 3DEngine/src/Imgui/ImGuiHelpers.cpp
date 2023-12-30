#pragma once

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include <glm/glm.hpp>

#include <string>
#include "ImGuiHelpers.h"

namespace Engine
{
const char *_labelPrefix(const char *const label, const char *field)
{
    float width = ImGui::CalcItemWidth();
    float x = ImGui::GetCursorPosX();
    ImGui::Text(label);
    ImGui::SameLine();
    ImGui::SetCursorPosX(x + width * 0.6f + ImGui::GetStyle().ItemInnerSpacing.x);
    ImGui::SetNextItemWidth(-1);

    std::string labelID = std::string("##") + label;
    // copy labelID into a char array
    char *labelIDChar = new char[labelID.size() + 1];
    std::copy(labelID.begin(), labelID.end(), labelIDChar);
    labelIDChar[labelID.size()] = '\0';

    if (strlen(field) > 0) return field;
    return labelIDChar;
}

void _drawVec3Control(const std::string &label, glm::vec3 &values, float resetValue)
{
    ImGuiIO &io = ImGui::GetIO();
    auto boldFont = io.Fonts->Fonts[0];
    ImGuiStyle &style = ImGui::GetStyle();

    // TODO: make font size (24.0f) a global
    float lineHeight = 24.0f + style.FramePadding.y * 2.0f;
    ImVec2 buttonSize = {2.0f, lineHeight};

    float width = ImGui::CalcItemWidth();

    ImGui::PushID(label.c_str());

    float x = ImGui::GetCursorPosX();
    ImGui::Text(label.c_str());
    ImGui::SameLine();
    ImGui::SetCursorPosX(x + width * 0.6f + ImGui::GetStyle().ItemInnerSpacing.x);
    ImGui::SetNextItemWidth(-1);

    auto itemWidth = ImGui::CalcItemWidth() / 3.0f - buttonSize.x - style.ItemInnerSpacing.x * (1 / 3);

    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{0, 0});

    ImGui::PushItemWidth(itemWidth);
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.8f, 0.1f, 0.15f, 1.0f});
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{0.9f, 0.2f, 0.2f, 1.0f});
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{0.8f, 0.1f, 0.15f, 1.0f});
    ImGui::PushFont(boldFont);
    if (ImGui::Button("", buttonSize)) values.x = resetValue;
    ImGui::PopFont();
    ImGui::PopStyleColor(3);

    ImGui::SameLine();
    ImGui::DragFloat("##X", &values.x, 0.1f, 0.0f, 0.0f, "%.2f");
    ImGui::PopItemWidth();
    ImGui::SameLine();

    ImGui::PushItemWidth(itemWidth);
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.2f, 0.7f, 0.2f, 1.0f});
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{0.3f, 0.8f, 0.3f, 1.0f});
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{0.2f, 0.7f, 0.2f, 1.0f});
    ImGui::PushFont(boldFont);
    if (ImGui::Button("", buttonSize)) values.y = resetValue;
    ImGui::PopFont();
    ImGui::PopStyleColor(3);

    ImGui::SameLine();
    ImGui::DragFloat("##Y", &values.y, 0.1f, 0.0f, 0.0f, "%.2f");
    ImGui::PopItemWidth();
    ImGui::SameLine();

    ImGui::PushItemWidth(itemWidth);
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.1f, 0.25f, 0.8f, 1.0f});
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{0.2f, 0.35f, 0.9f, 1.0f});
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{0.1f, 0.25f, 0.8f, 1.0f});
    ImGui::PushFont(boldFont);
    if (ImGui::Button("", buttonSize)) values.z = resetValue;
    ImGui::PopFont();
    ImGui::PopStyleColor(3);

    ImGui::SameLine();
    ImGui::DragFloat("##Z", &values.z, 0.1f, 0.0f, 0.0f, "%.2f");
    ImGui::PopItemWidth();

    ImGui::PopStyleVar();

    ImGui::Spacing();

    ImGui::PopID();
}

void _collapsingHeaderStyle(bool closed)
{
    ImGuiStyle &style = ImGui::GetStyle();

    // Customize the style for a normal state
    ImVec4 normalHeaderBgColor = ImVec4(0.2f, 0.2f, 0.2f, 0.5f);
    style.Colors[ImGuiCol_Header] = normalHeaderBgColor;

    // Customize the style for a hovered/clicked state
    ImVec4 hoveredHeaderBgColor = ImVec4(0.3f, 0.3f, 0.3f, 1.0f);
    style.Colors[ImGuiCol_HeaderHovered] = hoveredHeaderBgColor;

    if (!closed) ImGui::SetNextItemOpen(true, ImGuiCond_Once);
}
} // namespace Engine
