#include "ImGuiTextEditor.h"

#include <fstream>
#include "Project.h"
#include "InputManager.h"

namespace Engine
{
void ImGuiTextEditor::Init(const std::filesystem::path &filepath)
{
    m_FileToEdit = filepath;
    m_RelativePath = std::filesystem::relative(filepath, Project::GetAssetDirectory());
    m_Editor = new TextEditor();

    auto lang =
        filepath.extension() == ".lua" ? TextEditor::LanguageDefinition::Lua() : TextEditor::LanguageDefinition::GLSL();
    m_Editor->SetLanguageDefinition(lang);

    // TODO: do other things maybe

    std::ifstream file(filepath);
    if (file.good())
    {
        std::string str((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        m_Editor->SetText(str);
    }
}

void ImGuiTextEditor::Render()
{
    if (m_Editor == nullptr) return;
    m_Open = true;

    auto cpos = m_Editor->GetCursorPosition();

    // if ctrl + S is pressed, save the file
    if (InputManager::Get().IsKeyPressed(InputKey::S) &&
        InputManager::Get().IsKeyPressed(InputKey::LeftControl))
    {
        if (m_IsWindowFocused) SaveFile();
    }

    ImGui::Begin(m_RelativePath.string().c_str(), &m_Open,
                 ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_HorizontalScrollbar);

    ImGui::SetWindowSize(ImVec2(800, 600), ImGuiCond_FirstUseEver);
    m_IsWindowFocused = ImGui::IsWindowFocused();

    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Save"))
            {
                SaveFile();
            }
            if (ImGui::MenuItem("Quit", "Alt-F4"))
            {
                m_Open = false;
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Edit"))
        {
            bool ro = m_Editor->IsReadOnly();
            if (ImGui::MenuItem("Read-only mode", nullptr, &ro)) m_Editor->SetReadOnly(ro);
            ImGui::Separator();

            if (ImGui::MenuItem("Undo", "ALT-Backspace", nullptr, !ro && m_Editor->CanUndo())) m_Editor->Undo();
            if (ImGui::MenuItem("Redo", "Ctrl-Y", nullptr, !ro && m_Editor->CanRedo())) m_Editor->Redo();

            ImGui::Separator();

            if (ImGui::MenuItem("Copy", "Ctrl-C", nullptr, m_Editor->HasSelection())) m_Editor->Copy();
            if (ImGui::MenuItem("Cut", "Ctrl-X", nullptr, !ro && m_Editor->HasSelection())) m_Editor->Cut();
            if (ImGui::MenuItem("Delete", "Del", nullptr, !ro && m_Editor->HasSelection())) m_Editor->Delete();
            if (ImGui::MenuItem("Paste", "Ctrl-V", nullptr, !ro && ImGui::GetClipboardText() != nullptr))
                m_Editor->Paste();

            ImGui::Separator();

            if (ImGui::MenuItem("Select all", nullptr, nullptr))
                m_Editor->SetSelection(TextEditor::Coordinates(),
                                       TextEditor::Coordinates(m_Editor->GetTotalLines(), 0));

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("View"))
        {
            if (ImGui::MenuItem("Dark palette")) m_Editor->SetPalette(TextEditor::GetDarkPalette());
            if (ImGui::MenuItem("Light palette")) m_Editor->SetPalette(TextEditor::GetLightPalette());
            if (ImGui::MenuItem("Retro blue palette")) m_Editor->SetPalette(TextEditor::GetRetroBluePalette());
            ImGui::EndMenu();
        }

        // put slider to far right
        ImGui::SetCursorPosX(ImGui::GetWindowWidth() - 110);
        ImGui::PushItemWidth(100);
        ImGui::SliderFloat("##ss", &m_FontSize, 1.0f, 3.0f, "%.1f");
        ImGui::PopItemWidth();

        ImGui::EndMenuBar();
    }

    ImGui::Text("%6d/%-6d %6d lines  | %s | %s | %s | %s", cpos.mLine + 1, cpos.mColumn + 1, m_Editor->GetTotalLines(),
                m_Editor->IsOverwrite() ? "Ovr" : "Ins", m_Editor->CanUndo() ? "*" : " ",
                m_Editor->GetLanguageDefinition().mName.c_str(), m_RelativePath.string().c_str());

    // change imgui font size

    ImGuiIO &io = ImGui::GetIO();
    auto font = io.Fonts->Fonts[2];
    float oldSize = font->Scale;
    font->Scale *= m_FontSize;
    ImGui::PushFont(font);

    m_Editor->Render(m_RelativePath.string().c_str());

    ImGui::GetFont()->Scale = oldSize;
    ImGui::PopFont();

    ImGui::End();

    if (!m_Open) m_Editor = nullptr;
}

void ImGuiTextEditor::SaveFile()
{
    auto textToSave = m_Editor->GetText();
    /// save text....
    std::ofstream file(m_FileToEdit);
    if (file.good())
    {
        file.write(textToSave.c_str(), textToSave.size());
        file.flush();
    }
}
} // namespace Engine