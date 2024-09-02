#include "ConsolePanel.h"

#include <imgui.h>

namespace Engine
{
ConsolePanel::ConsolePanel() {}

void ConsolePanel::OnImGuiRender() 
{
	ImGui::Begin("Console");

	ImGui::End();
}
} // namespace Engine
