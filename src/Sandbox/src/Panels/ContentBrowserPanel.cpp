#include "ContentBrowserPanel.h"

#include <imgui.h>

#include "ResourceManager.h"
#include "Project.h"

namespace Engine
{
ContentBrowserPanel::ContentBrowserPanel()
    : m_BaseDirectory("/home/adeleye/Source/3DEngine/src/Sandbox/res"), m_CurrentDirectory(m_BaseDirectory)
{
    auto basePath = "/home/adeleye/Source/3DEngine/src/Sandbox/Resources/ContentBrowser";
    m_DirectoryIcon = ResourceManager::Instance().LoadTexture(basePath + std::string("/DirectoryIcon.png"));
    m_FileIcon = ResourceManager::Instance().LoadTexture(basePath + std::string("/FileIcon.png"));
}

void ContentBrowserPanel::OnImGuiRender()
{
    ImGui::Begin("Content Browser");

    if (m_CurrentDirectory != std::filesystem::path(m_BaseDirectory))
    {
        if (ImGui::Button("<-"))
        {
            m_CurrentDirectory = m_CurrentDirectory.parent_path();
        }
    }

    static float padding = 16.0f;
    static float thumbnailSize = 128.0f;
    float cellSize = thumbnailSize + padding;

    float panelWidth = ImGui::GetContentRegionAvail().x;
    int columnCount = (int)(panelWidth / cellSize);
    if (columnCount < 1) columnCount = 1;

    ImGui::Columns(columnCount, 0, false);

    for (auto &directoryEntry : std::filesystem::directory_iterator(m_CurrentDirectory))
    {
        const auto &path = directoryEntry.path();
        std::string filenameString = path.filename().string();

        ImGui::PushID(filenameString.c_str());
        auto icon = directoryEntry.is_directory() ? m_DirectoryIcon : m_FileIcon;
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
        ImGui::ImageButton((ImTextureID)icon, {thumbnailSize, thumbnailSize}, {1, 0}, {0, 1});

        if (ImGui::BeginDragDropSource())
        {
            std::filesystem::path relativePath(path);
            const char *itemPath = relativePath.c_str();
            ImGui::SetDragDropPayload("CONTENT_BROWSER_ITEM", itemPath, (strlen(itemPath) + 1) * sizeof(wchar_t));
            ImGui::EndDragDropSource();
        }

        ImGui::PopStyleColor();
        if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
        {
            if (directoryEntry.is_directory()) m_CurrentDirectory /= path.filename();
        }
        ImGui::TextWrapped(filenameString.c_str());

        ImGui::NextColumn();

        ImGui::PopID();
    }

    ImGui::Columns(1);

    // ImGui::SliderFloat("Thumbnail Size", &thumbnailSize, 16, 512);
    // ImGui::SliderFloat("Padding", &padding, 0, 32);

    // TODO: status bar
    ImGui::End();
}
} // namespace Engine