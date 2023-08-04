#include "ContentBrowserPanel.h"

#include <imgui.h>

#include "ResourceManager.h"
#include "Project.h"
#include "TextureImporter.h"
#include "AssetManager.h"
#include "Log.h"
#include "PlatformUtils.h"

#include <iostream>

namespace Engine
{

ContentBrowserPanel::ContentBrowserPanel()
    : m_BaseDirectory(Project::GetAssetDirectory()), m_CurrentDirectory(m_BaseDirectory)
{
    auto basePath = "/home/adeleye/Source/3DEngine/src/Sandbox/Resources/Icons/ContentBrowser";
    m_DirectoryIcon = TextureImporter::LoadTexture2D(basePath + std::string("/DirectoryIcon.png"));
    m_FileIcon = TextureImporter::LoadTexture2D(basePath + std::string("/FileIcon.png"));

    RefreshAssetTree();

    m_Mode = Mode::Asset;
}

void ContentBrowserPanel::OnImGuiRender()
{
    ImGui::Begin("Content Browser");

    const char *label = m_Mode == Mode::Asset ? "Asset" : "File";
    if (ImGui::Button(label)) m_Mode = m_Mode == Mode::Asset ? Mode::FileSystem : Mode::Asset;

    if (m_CurrentDirectory != std::filesystem::path(m_BaseDirectory) && m_Mode == Mode::FileSystem)
    {
        ImGui::SameLine();
        if (ImGui::Button("<-"))
        {
            m_CurrentDirectory = m_CurrentDirectory.parent_path();
        }
    }

    if (!m_NodeStack.empty() && m_Mode == Mode::Asset)
    {
        ImGui::SameLine();
        if (ImGui::Button("<-"))
        {
            currentNode = m_NodeStack.back();
            m_NodeStack.pop_back();
            m_AssetCurrentDirectory = m_AssetCurrentDirectory.parent_path();
        }
    }

    static float padding = 20.0f;
    static float thumbnailSize = 150.0f;
    float cellSize = thumbnailSize + padding;

    float panelWidth = ImGui::GetContentRegionAvail().x;
    int columnCount = (int)(panelWidth / cellSize);
    if (columnCount < 1) columnCount = 1;

    ImGui::Columns(columnCount, 0, false);

    if (m_Mode == Mode::Asset)
    {

        // Right-click on blank space
        if (ImGui::BeginPopupContextWindow())
        {
            if (ImGui::MenuItem("Refresh")) RefreshAssetTree();
            if (ImGui::MenuItem("Open in File Browser"))
            {
                std::string path = Project::GetAssetDirectory() / m_AssetCurrentDirectory;
                Utils::FileDialogs::OpenFileExplorer(path + "/");
            }
            ImGui::EndPopup();
        }

        for (auto &[name, childNode] : currentNode->Children)
        {
            bool isDirectory = !childNode.Children.empty();
            bool opened = false;

            ImGui::PushID(name.c_str());
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
            Texture2DRef icon = isDirectory ? m_DirectoryIcon : m_FileIcon;
            ImGui::ImageButton((ImTextureID)icon->GetRendererID(), {thumbnailSize, thumbnailSize}, {0, 1}, {1, 0});

            if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
            {
                if (isDirectory)
                {
                    opened = true;
                    // Push the current node onto the stack
                    m_NodeStack.push_back(currentNode);
                    m_AssetCurrentDirectory /= std::filesystem::path(name);
                }
            }

            if (ImGui::BeginDragDropSource() && !isDirectory)
            {
                auto completePath = m_AssetCurrentDirectory / std::filesystem::path(name);
                auto assetHandle = m_AssetHandles[completePath.string()].ToString();
                ImGui::SetDragDropPayload("CONTENT_BROWSER_ITEM", assetHandle.c_str(),
                                          (strlen(assetHandle.c_str()) + 1) * sizeof(char *));
                ImGui::Button(completePath.c_str());
                ImGui::EndDragDropSource();
            }

            ImGui::TextWrapped(name.c_str());

            if (opened) currentNode = &childNode;

            ImGui::PopStyleColor();
            ImGui::PopID();
            ImGui::NextColumn();
        }
    }
    else
    {
        // Right-click on blank space
        if (ImGui::BeginPopupContextWindow())
        {
            if (ImGui::MenuItem("Open in File Browser"))
                Utils::FileDialogs::OpenFileExplorer(m_CurrentDirectory.string() + "/");
            ImGui::EndPopup();
        }
        for (auto &directoryEntry : std::filesystem::directory_iterator(m_CurrentDirectory))
        {
            const auto &path = directoryEntry.path();
            std::string filenameString = path.filename().string();

            ImGui::PushID(filenameString.c_str());
            Texture2DRef icon = directoryEntry.is_directory() ? m_DirectoryIcon : m_FileIcon;
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
            ImGui::ImageButton((ImTextureID)icon->GetRendererID(), {thumbnailSize, thumbnailSize}, {0, 1}, {1, 0});

            if (!directoryEntry.is_directory() && ImGui::BeginPopupContextItem())
            {
                if (ImGui::MenuItem("Import"))
                {
                    auto relativePath = std::filesystem::relative(path, Project::GetAssetDirectory());
                    Project::GetActive()->GetEditorAssetManager()->ImportAsset(relativePath);
                }
                ImGui::EndPopup();
            }

            if (ImGui::BeginDragDropSource())
            {
                auto relativePath = std::filesystem::relative(path, Project::GetAssetDirectory());
                const char *itemPath = relativePath.c_str();
                ImGui::SetDragDropPayload("CONTENT_BROWSER_ITEM", itemPath, (strlen(itemPath) + 1) * sizeof(char *));
                ImGui::Button(relativePath.c_str());
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
    }

    ImGui::Columns(1);

    // ImGui::SliderFloat("Thumbnail Size", &thumbnailSize, 16, 512);
    // ImGui::SliderFloat("Padding", &padding, 0, 32);

    // TODO: status bar
    ImGui::End();
}

void ContentBrowserPanel::RefreshAssetTree()
{
    const auto &assetRegistry = Project::GetActive()->GetEditorAssetManager()->GetAssetRegistry();
    for (const auto &[handle, metadata] : assetRegistry)
    {
        m_AssetHandles[metadata.FilePath.string()] = handle;
        AddPathToTree(m_FileTree, metadata.FilePath);
    }
}

void ContentBrowserPanel::AddPathToTree(FileTreeNode &root, const std::filesystem::path &path)
{
    FileTreeNode *current = &root;

    for (const auto &component : path)
    {
        current = &(current->Children[component.string()]);
    }
}

} // namespace Engine