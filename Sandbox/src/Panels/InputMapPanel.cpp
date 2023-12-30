#include "InputMapPanel.h"

#include "IconsFontAwesome5.h"

#include <imgui.h>
#include "Log.h"
#include "InputManager.h"
#include "Project.h"

#include <ImGuiHelpers.h>

namespace Engine
{
bool showModal = false;
InputKey SelectedKey = InputKey::None;
MouseButton SelectedMouseButton = MouseButton::None;
bool ctrl, alt, shift, super;
std::string currentActionName;
InputToActionMapVariant currentInput;
bool isEditing = false;

static void Reset()
{
    SelectedKey = InputKey::None;
    SelectedMouseButton = MouseButton::None;
    ctrl = alt = shift = super = false;
}

static SpecialKey GetSpecialKeyFromCombination()
{
    if (ctrl && alt && shift && super) return SpecialKey::ShiftControlAltSuper;
    if (!ctrl && alt && shift && super) return SpecialKey::ShiftAltSuper;
	if (ctrl && !alt && shift && super) return SpecialKey::ShiftControlSuper;
	if (ctrl && alt && !shift && super) return SpecialKey::ControlAltSuper;
	if (ctrl && alt && shift && !super) return SpecialKey::ShiftControlAlt;
	if (!ctrl && !alt && shift && super) return SpecialKey::ShiftSuper;
	if (!ctrl && alt && !shift && super) return SpecialKey::AltSuper;
	if (!ctrl && alt && shift && !super) return SpecialKey::ShiftAlt;
	if (ctrl && !alt && !shift && super) return SpecialKey::ControlSuper;
	if (ctrl && !alt && shift && !super) return SpecialKey::ShiftControl;
	if (ctrl && alt && !shift && !super) return SpecialKey::ControlAlt;
	if (!ctrl && !alt && !shift && super) return SpecialKey::Super;
	if (!ctrl && !alt && shift && !super) return SpecialKey::Shift;
	if (!ctrl && alt && !shift && !super) return SpecialKey::Alt;
	if (ctrl && !alt && !shift && !super) return SpecialKey::Control;

	return SpecialKey::None;
}

void InputMapPanel::OnImGuiRender()
{
    if (!m_IsOpen) return;

    ImGui::Begin("Input Map", &m_IsOpen);

    // textbox for action name
    static char actionName[128] = "Add New Action";

    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5, 10));
    ImGui::PushItemWidth(ImGui::GetWindowWidth() - 150);
    ImGui::InputText("##actionName", actionName, IM_ARRAYSIZE(actionName));
    ImGui::PopItemWidth();
    ImGui::SameLine();
    if (ImGui::Button("Add", ImVec2(120, 0)))
    {
        const std::string actionNameStr(actionName);
        InputManager::Get().MapInputToAction(actionNameStr, std::make_pair(SpecialKey::None, InputKey::None));
    }
    ImGui::PopStyleVar();

    ImGui::Separator();

    if (ImGui::BeginTable("testtb", 2, ImGuiTableFlags_SizingFixedFit))
    {
        ImGui::TableSetupColumn("##AAA", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("##BBB", ImGuiTableColumnFlags_WidthFixed, 80.0f);

        for (const auto &action : InputManager::Get().GetInputToActionMap())
        {
            ImGui::TableNextRow();

            ImGui::TableNextColumn();
            _collapsingHeaderStyle();
            ImGui::PushID(action.first.c_str());
            if (ImGui::CollapsingHeader(action.first.c_str()))
            {
                ImGui::Indent();
                for (const auto &input : action.second)
                {
                    if (input.index() == 0)
                    {
                        const auto &key = std::get<std::pair<SpecialKey, InputKey>>(input);
                        if (key.first == SpecialKey::None && key.second == InputKey::None) continue;

                        if (key.first == SpecialKey::None)
                        {
                            ImGui::Text(("Keyboard " + Utils::InputKeyToString(key.second)).c_str());
                        }
                        else
                        {
                            ImGui::Text((Utils::SpecialKeyToString(key.first) +
                                         " + " +
                                         "Keyboard " +
                                         Utils::InputKeyToString(key.second)).c_str());
                        }
                    }
                    else if (input.index() == 1)
                    {
                        const auto &key = std::get<std::pair<SpecialKey, MouseButton>>(input);
                        if (key.first == SpecialKey::None && key.second == MouseButton::None) continue;

                        if (key.first == SpecialKey::None)
                        {
                            ImGui::Text(("Mouse Button " + Utils::MouseButtonToString(key.second)).c_str());
                        }
                        else
                        {
                            ImGui::Text((Utils::SpecialKeyToString(key.first) +
                                         " + " +
                                         "Mouse Button " +
                                         Utils::MouseButtonToString(key.second)).c_str());
                        }
                    }
                    ImGui::SameLine();

                    ImGui::SetCursorPosX(ImGui::GetWindowWidth() - 160);
                    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5, 6));
                    if (ImGui::Button(ICON_FA_PENCIL_ALT))
                    {
						currentActionName = action.first;
                        currentInput = input;
						isEditing = true;
						showModal = true;
                    }
                    ImGui::SameLine();
                    if (ImGui::Button(ICON_FA_MINUS))
                    {
                        InputManager::Get().UnmapInputFromAction(action.first, input);
						Project::SaveActive(Project::GetProjectConfigPath());
                    }
                    ImGui::PopStyleVar();
                }
                ImGui::Unindent();
            }

            ImGui::TableNextColumn();

            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5, 5));
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 10);
            if (ImGui::Button(ICON_FA_PLUS))
            {
                currentActionName = action.first;
                showModal = true;
            }
            ImGui::SameLine();
			if (ImGui::Button(ICON_FA_TRASH))
			{
                InputManager::Get().RemoveAction(action.first);
                Project::SaveActive(Project::GetProjectConfigPath());
			}

            ImGui::PopStyleVar();
            ImGui::PopID();
        }
        ImGui::EndTable();
    }

    ImGui::End();

    // Modal popup
    if (showModal)
    {
        ImGui::OpenPopup("Event Configuration");
        showModal = false; // Reset the flag after opening the popup
    }

    if (ImGui::BeginPopupModal("Event Configuration", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        // Contents of the modal popup
        std::string config = "No Event Configuration";

		if (SelectedKey != InputKey::None)
        {
            config = GetSpecialKeyFromCombination() != SpecialKey::None
                              ? Utils::SpecialKeyToString(GetSpecialKeyFromCombination()) + " + " + Utils::InputKeyToString(SelectedKey)
                              : Utils::InputKeyToString(SelectedKey);
        }
		else if (SelectedMouseButton != MouseButton::None)
		{
            config = GetSpecialKeyFromCombination() != SpecialKey::None
                         ? Utils::SpecialKeyToString(GetSpecialKeyFromCombination()) + " + " + Utils::MouseButtonToString(SelectedMouseButton)
                         : Utils::MouseButtonToString(SelectedMouseButton);
		}
		 
        //ImGui::SetCursorPosX(ImGui::GetWindowWidth() / 2 - ImGui::CalcTextSize(config.c_str()).x / 2);
		ImGui::Text("%s", config.c_str());

        ImGui::Separator();

        static char buffer[128] = "Listening for input...";
        ImGui::InputText("##event", buffer, IM_ARRAYSIZE(buffer), ImGuiInputTextFlags_ReadOnly);
        ImGui::SameLine();
        if (ImGui::Button(ICON_FA_TIMES)) SelectedKey = InputKey::None;

        // select input type from list
        static const char *inputTypes[] = {"Keyboard Keys", "Mouse Buttons", "Joystick Buttons", "Joystick Axes"};
        static int inputType = 0;
        if (ImGui::Combo("##inputType", &inputType, inputTypes, IM_ARRAYSIZE(inputTypes))) Reset();
        if (inputType == 0)
        {
            const auto key = InputManager::Get().GetKey();
            if (key != InputKey::None)
            {
                strcpy_s(buffer,
                         std::format("{0} or {0} (Physical) or {0} (Unicode)", Utils::InputKeyToString(SelectedKey))
                             .c_str());
                SelectedKey = key;
                SelectedMouseButton = MouseButton::None;
            }
            if (SelectedKey == InputKey::None)
            {
                strcpy_s(buffer, "Listening for input...");
            }
        }
		else if (inputType == 1)
		{
			const auto button = InputManager::Get().GetMouseButton();
			if (button != MouseButton::None)
			{
				strcpy_s(buffer, std::format("{0} Mouse Button", Utils::MouseButtonToString(button)).c_str());
				SelectedMouseButton = button;
				SelectedKey = InputKey::None;
			}
			if (SelectedMouseButton == MouseButton::None)
			{
				strcpy_s(buffer, "Listening for input...");
			}
		}
        ImGui::Separator();
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 40);

        if (SelectedKey != InputKey::None || SelectedMouseButton != MouseButton::None)
        {
            ImGui::Text("Additional Configuration");
            // radio buttons for ctrl, alt, shift, super
            ImGui::Checkbox("Ctrl", &ctrl);
            ImGui::SameLine();
            ImGui::Checkbox("Alt", &alt);
            ImGui::SameLine();
            ImGui::Checkbox("Shift", &shift);
            ImGui::SameLine();
            ImGui::Checkbox("Super", &super);
        }

        // Close the popup when the button is clicked
        if (ImGui::Button("OK"))
        {
			if (isEditing)
			{
				InputManager::Get().UpdateInputToActionMap(currentActionName, currentInput, 
					std::make_pair(GetSpecialKeyFromCombination(), SelectedKey));
				isEditing = false;
			} 
			else
			{
				if (inputType == 0)
				{
					InputManager::Get().MapInputToAction(
						currentActionName, std::make_pair(GetSpecialKeyFromCombination(), SelectedKey));
				}
				else if (inputType == 1)
				{
					InputManager::Get().MapInputToAction(
						currentActionName,
						std::make_pair(GetSpecialKeyFromCombination(), SelectedMouseButton));
				}
			}

			Project::SaveActive(Project::GetProjectConfigPath());
            Reset();
            currentActionName = "";
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("Close"))
        {
            Reset();
            currentActionName = "";
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
}
} // namespace Engine
