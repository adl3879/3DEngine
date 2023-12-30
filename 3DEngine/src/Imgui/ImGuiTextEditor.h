#pragma once

#include <filesystem>
#include <memory>

#include <TextEditor.h>

namespace Engine
{
class ImGuiTextEditor
{
  public:
    ImGuiTextEditor() = default;
    ~ImGuiTextEditor() = default;

    void Init(const std::filesystem::path &filepath);
    void Render();

    bool IsWindowFocused() const { return m_IsWindowFocused; }

  private:
    void SaveFile();

  private:
    TextEditor* m_Editor;
    std::filesystem::path m_FileToEdit, m_RelativePath;
    bool m_Open = true;
    float m_FontSize = 1.0f;

    bool m_IsWindowFocused = false;
};

using ImGuiTextEditorRef = std::shared_ptr<ImGuiTextEditor>;
} // namespace Engine