#include "ContentBrowserPanel.h"

#include <imgui.h>

#include "ResourceManager.h"
#include "Project.h"
#include "TextureImporter.h"
#include "AssetManager.h"
#include "Log.h"
#include "PlatformUtils.h"
#include "IconsFontAwesome5.h"
#include "MaterialEditorPanel.h"

#include <iostream>
#include <fstream>

namespace Engine
{
bool openCreateFilePopup = false;

namespace Utils
{
static void CreateFilePopUp(const std::filesystem::path &currentDirectory, bool open = true)
{
    if (open) ImGui::OpenPopup("Create File");

    if (ImGui::BeginPopupModal("Create File", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        static char name[128] = {0};
        ImGui::InputText("Name", name, IM_ARRAYSIZE(name));
        if (ImGui::Button("OK", ImVec2(120, 0)))
        {
            std::string path = Project::GetAssetDirectory() / currentDirectory / name;
            std::ofstream file(path);
            file.close();
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(120, 0))) ImGui::CloseCurrentPopup();
        ImGui::EndPopup();
    }
}
} // namespace Utils

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

    static float padding = 20.0f;
    static float thumbnailSize = 150.0f;
    float cellSize = thumbnailSize + padding;

    float panelWidth = ImGui::GetContentRegionAvail().x;
    int columnCount = (int)(panelWidth / cellSize);
    if (columnCount < 1) columnCount = 1;

    ImGui::BeginChild("Directory Tree", ImVec2(panelWidth * 0.15f, 0), true, ImGuiWindowFlags_HorizontalScrollbar);
    DisplayFileHierarchy(m_BaseDirectory);
    ImGui::EndChild();

    ImGui::SameLine();

    ImGui::BeginChild("Content Region", ImVec2(panelWidth, 0), false, ImGuiWindowFlags_HorizontalScrollbar);
    const char *label = m_Mode == Mode::Asset ? "Asset" : "File";
    if (ImGui::Button(label)) m_Mode = m_Mode == Mode::Asset ? Mode::FileSystem : Mode::Asset;

    if (m_CurrentDirectory != std::filesystem::path(m_BaseDirectory) && m_Mode == Mode::FileSystem)
    {
        ImGui::SameLine();
        if (ImGui::Button(ICON_FA_CHEVRON_LEFT "   "))
        {
            m_CurrentDirectory = m_CurrentDirectory.parent_path();
        }
    }

    if (!m_NodeStack.empty() && m_Mode == Mode::Asset)
    {
        ImGui::SameLine();
        if (ImGui::Button(ICON_FA_CHEVRON_LEFT "   "))
        {
            currentNode = m_NodeStack.back();
            m_NodeStack.pop_back();
            m_AssetCurrentDirectory = m_AssetCurrentDirectory.parent_path();
        }
    }

    ImGui::Columns(columnCount, 0, false);

    if (m_Mode == Mode::Asset)
    {
        // Right-click on blank space
        if (ImGui::BeginPopupContextWindow())
        {
            if (ImGui::MenuItem(ICON_FA_SYNC_ALT "   Refresh")) RefreshAssetTree();
            if (ImGui::MenuItem(ICON_FA_FOLDER_OPEN "   Open in File Browser"))
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

            auto completePath = m_AssetCurrentDirectory / std::filesystem::path(name);

            if (!isDirectory && ImGui::BeginPopupContextItem())
            {
                if (ImGui::MenuItem(ICON_FA_MINUS_CIRCLE "   Unload"))
                {
                    auto assetHandle = m_AssetHandles[completePath.string()];
                    AssetManager::UnloadAsset(assetHandle);
                    m_NodeStack.pop_back();
                    RefreshAssetTree();
                }
                ImGui::EndPopup();
            }

            if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
            {
                if (isDirectory)
                {
                    opened = true;
                    // Push the current node onto the stack
                    m_NodeStack.push_back(currentNode);
                    m_AssetCurrentDirectory /= std::filesystem::path(name);
                }
                else
                {
                    auto extension = completePath.extension().string();
                    if (extension == ".material")
                    {
                        auto assetHandle = m_AssetHandles[completePath.string()];
                        MaterialEditorPanel::OpenMaterialEditor(assetHandle);
                    }
                }
            }

            if (ImGui::BeginDragDropSource() && !isDirectory)
            {
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
            if (ImGui::BeginMenu(ICON_FA_PLUS "  Add Item"))
            {
                if (ImGui::MenuItem(ICON_FA_PHOTO_VIDEO "   Scene")) OpenCreateFilePopup(AssetType::Scene);
                if (ImGui::MenuItem(ICON_FA_PAINT_BRUSH "   Material")) OpenCreateFilePopup(AssetType::Material);
                ImGui::EndPopup();
            }
            if (ImGui::MenuItem(ICON_FA_FOLDER_OPEN "   Open in File Browser"))
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
                if (ImGui::MenuItem(ICON_FA_FILE_IMPORT "   Import"))
                {
                    auto relativePath = std::filesystem::relative(path, Project::GetAssetDirectory());
                    AssetManager::ImportAsset(relativePath);
                }
                if (ImGui::MenuItem(ICON_FA_TRASH "   Delete"))
                {
                    std::filesystem::remove(path);
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
    ImGui::EndChild();

    // ImGui::SliderFloat("Thumbnail Size", &thumbnailSize, 16, 512);
    // ImGui::SliderFloat("Padding", &padding, 0, 32);

    CreateFilePopup();

    // TODO: status bar
    ImGui::End();
}

void ContentBrowserPanel::DisplayFileHierarchy(const std::filesystem::path &directory)
{
    if (!fs::exists(directory) || !fs::is_directory(directory)) return;

    for (auto &directoryEntry : std::filesystem::directory_iterator(directory))
    {
        const fs::path &entryPath = directoryEntry.path();

        // Get the hash of the node label
        std::size_t labelHash = std::hash<std::string>{}(entryPath.filename().string());
        bool &isOpen = m_NodeOpenStatusMap[labelHash];

        // Check if the entry is a directory
        if (fs::is_directory(entryPath))
        {
            bool treeNodeOpen = ImGui::TreeNodeEx(entryPath.filename().string().c_str(),
                                                  ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick,
                                                  !isOpen ? ICON_FA_FOLDER "  %s" : ICON_FA_FOLDER_OPEN "  %s",
                                                  entryPath.filename().string().c_str());

            if (treeNodeOpen)
            {
                isOpen = true;
                DisplayFileHierarchy(entryPath);
                ImGui::TreePop();
            }
            else
                isOpen = false;
        }
        else if (fs::is_regular_file(entryPath))
        {
            ImGui::TreeNodeEx(entryPath.filename().string().c_str(), ImGuiTreeNodeFlags_Leaf, ICON_FA_FILE "  %s",
                              entryPath.filename().string().c_str());

            if (ImGui::BeginPopupContextItem())
            {
                if (ImGui::MenuItem(ICON_FA_FILE_IMPORT "   Import"))
                {
                    auto relativePath = std::filesystem::relative(entryPath, Project::GetAssetDirectory());
                    Project::GetActive()->GetEditorAssetManager()->ImportAsset(relativePath);
                }
                ImGui::EndPopup();
            }
            ImGui::TreePop();
        }
    }
}

void ContentBrowserPanel::CreateFilePopup()
{
    if (m_OpenCreateFilePopup)
    {
        ImGui::OpenPopup("create_scene_file");
        m_OpenCreateFilePopup = false;
    }
    if (ImGui::BeginPopup("create_scene_file"))
    {
        static char name[128] = "New Scene\0";
        ImGui::Text(ICON_FA_PHOTO_VIDEO "   Create Scene");
        ImGui::Separator();
        ImGui::InputText(" ", name, IM_ARRAYSIZE(name));
        ImGui::SameLine();
        if (ImGui::Button(ICON_FA_CHECK "  Save", ImVec2(100, 0)))
        {
            std::string path = m_CurrentDirectory / name;
            std::string extension = m_NewAssetType == AssetType::Scene      ? ".scene"
                                    : m_NewAssetType == AssetType::Material ? ".material"
                                                                            : "";
            std::ofstream file(path + extension);
            AssetManager::ImportAsset(path + extension);
            file.close();
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
}

void ContentBrowserPanel::OpenCreateFilePopup(AssetType type)
{
    m_NewAssetType = type;
    m_OpenCreateFilePopup = true;
}

void ContentBrowserPanel::RefreshAssetTree()
{
    const auto &assetRegistry = AssetManager::GetRegistry();
    for (const auto &[handle, metadata] : assetRegistry)
    {
        m_AssetHandles[metadata.FilePath.string()] = handle;
        AddPathToTree(m_FileTree, metadata.FilePath);
    }
}

void ContentBrowserPanel::AddPathToTree(FileTreeNode &root, const std::filesystem::path &path)
{
    FileTreeNode *current = &root;
    for (const auto &component : path) current = &(current->Children[component.string()]);
}

} // namespace Engine