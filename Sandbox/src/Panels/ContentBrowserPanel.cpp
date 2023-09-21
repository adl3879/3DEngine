#include "ContentBrowserPanel.h"

#include <imgui.h>

#include "Project.h"
#include "TextureImporter.h"
#include "AssetManager.h"
#include "Log.h"
#include "IconsFontAwesome5.h"
#include "MaterialEditorPanel.h"
#include "Utils/FileDialogs.h"

#include <iostream>
#include <fstream>
#include <cctype>


namespace Engine
{ 
bool openCreateFilePopup = false;

static char searchStr[128] = "";

Texture2DRef sceneIcon, backIcon, forwardIcon;

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
            auto path = Project::GetAssetDirectory() / currentDirectory / name;
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
    m_DirectoryIcon = TextureImporter::LoadTexture2D("Resources/Icons/ContentBrowser/DirectoryIcon.png");
    m_FileIcon = TextureImporter::LoadTexture2D("Resources/Icons/ContentBrowser/FileIcon.png");
	sceneIcon = TextureImporter::LoadTexture2D("Resources/Icons/ContentBrowser/SceneIcon.png");
	backIcon = TextureImporter::LoadTexture2D("Resources/Icons/ContentBrowser/BackIcon.png");
	forwardIcon = TextureImporter::LoadTexture2D("Resources/Icons/ContentBrowser/ForwardIcon.png");

    RefreshAssetTree();

    m_Mode = Mode::FileSystem;
}

void ContentBrowserPanel::OnImGuiRender()
{
    ImGui::Begin("Content Browser");

    static float padding = 50.0f;
    static float thumbnailSize = 160.0f;
    float cellSize = thumbnailSize + padding;

    float panelWidth = ImGui::GetContentRegionAvail().x;
    float dirTreeWidth = panelWidth * 0.18;
    int columnCount = (int)(panelWidth / cellSize);
    if (columnCount < 1) columnCount = 1;

    ImGui::BeginChild("Directory Tree", ImVec2(dirTreeWidth, 0), true, ImGuiWindowFlags_HorizontalScrollbar);
    DisplayFileHierarchy(m_BaseDirectory);
    ImGui::EndChild();

    ImGui::SameLine();

    ImGui::BeginChild("Content Region", ImVec2(panelWidth - dirTreeWidth, 0), false,
                      ImGuiWindowFlags_HorizontalScrollbar);

	ImGui::Begin("##content_browser_toolbar", nullptr, 
		ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoScrollbar);

	ImGui::Text("Content Browser");
	ImGui::SameLine();
	ImGui::TextColored({0.5f, 0.5f, 0.5f, 1.0f}, "%s", m_CurrentDirectory.string().c_str());

	ImGui::SameLine();

	// volume slider to control tumbnail size
	// float to right
	ImGui::SetCursorPosX(ImGui::GetWindowWidth() - 150);

	ImGui::SliderFloat("##thumbnail_size", &thumbnailSize, 100, 512);
    ImGui::End();

	ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10);
	if (ImGui::ImageButton((void *)(intptr_t)backIcon->GetRendererID(), {28, 22}, {0, 1}, {1, 0}))
	{
		if (m_CurrentDirectory != m_BaseDirectory)
		{
            m_DirectoryStack.push_back(m_CurrentDirectory.stem());
			m_CurrentDirectory = m_CurrentDirectory.parent_path();
		}
	}
	ImGui::SameLine();
	
	if (ImGui::ImageButton((void *)(intptr_t)forwardIcon->GetRendererID(), {28, 22}, {0, 1}, {1, 0}))
	{
		if (!m_DirectoryStack.empty())
		{
			m_CurrentDirectory /= m_DirectoryStack.back();
			m_DirectoryStack.pop_back();
		}
	}
	ImGui::SameLine();

	// Search bar
	ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 10);
	ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 4);
    // search bar
    ImGui::PushItemWidth(350);
   
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5, 5));
	ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 10.0f);
	if (ImGui::InputTextWithHint("##Search", ICON_FA_SEARCH  "  Search", searchStr, IM_ARRAYSIZE(searchStr)))
	{
		Search(searchStr);
	}
	ImGui::PopStyleVar(2);

	ImGui::PopItemWidth();

	ImGui::SameLine();
	ImGui::SetCursorPosX(ImGui::GetWindowWidth() - 30);

	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.2, 0.2, 0.2, 0.2));
    if (ImGui::Button(ICON_FA_COG)) {}
	ImGui::PopStyleColor(2);

    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10);
	ImGui::Separator();
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10);

    ImGui::Columns(columnCount, 0, false);

    if (m_Mode == Mode::Asset)
    {
        // Right-click on blank space
        if (ImGui::BeginPopupContextWindow())
        {
            if (ImGui::MenuItem(ICON_FA_SYNC_ALT "   Refresh")) RefreshAssetTree();
            if (ImGui::MenuItem(ICON_FA_FOLDER_OPEN "   Open in File Browser"))
            {
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
            auto completePath = m_AssetCurrentDirectory / std::filesystem::path(name);
            auto extension = completePath.extension().string();

            if (extension == ".png" || extension == ".jpg" || extension == ".jpeg" || extension == ".tga")
                icon = AssetManager::GetAsset<Texture2D>(m_AssetHandles[completePath.string()]);

            if (icon)
            {
                ImGui::ImageButton((ImTextureID)icon->GetRendererID(), {thumbnailSize, thumbnailSize}, {0, 1}, {1, 0});
            }
            else
            {
                ImGui::Button("No Thumbnail", {thumbnailSize, thumbnailSize});
            }

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
                        auto &assetHandle = m_AssetHandles[completePath.string()];
                        MaterialEditorPanel::OpenMaterialEditor(assetHandle);
                    }
                }
            }

            if (ImGui::BeginDragDropSource() && !isDirectory)
            {
                auto assetHandle = m_AssetHandles[completePath.string()].ToString();
                ImGui::SetDragDropPayload("CONTENT_BROWSER_ITEM", assetHandle.c_str(),
                                          (strlen(assetHandle.c_str()) + 1) * sizeof(char *));
                ImGui::Button(completePath.string().c_str());
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
                if (ImGui::MenuItem(ICON_FA_FOLDER "   New Folder"))
                {

                };
                if (ImGui::MenuItem(ICON_FA_FILE "   New File")) OpenCreateFilePopup(AssetType::None);
                ImGui::Separator();
                if (ImGui::MenuItem(ICON_FA_PHOTO_VIDEO "   Scene")) OpenCreateFilePopup(AssetType::Scene);
                if (ImGui::MenuItem(ICON_FA_PAINT_BRUSH "   Material")) OpenCreateFilePopup(AssetType::Material);
                ImGui::EndPopup();
            }
            if (ImGui::BeginMenu(ICON_FA_FILE_IMPORT "   Import"))
            {
                if (ImGui::MenuItem(ICON_FA_IMAGE "   2D Texture"))
                {
                    // TODO: copy image file and load
                }
                ImGui::EndPopup();
            }
            if (ImGui::MenuItem(ICON_FA_FOLDER_OPEN "   Open in File Browser"))
            {
			
            }
            ImGui::EndPopup();
        }

		if (m_CurrentDirectoryEntries.empty())
        {
            for (auto &directoryEntry : std::filesystem::directory_iterator(m_CurrentDirectory))
                DrawFileAssetBrowser(directoryEntry);
        }
		else
		{
            for (auto &directoryEntry : m_CurrentDirectoryEntries) DrawFileAssetBrowser(directoryEntry);
		}
    }

    // ImGui::SliderFloat("Thumbnail Size", &thumbnailSize, 16, 512);
    // ImGui::SliderFloat("Padding", &padding, 0, 32);
    ImGui::Columns(1);
    ImGui::EndChild();

    CreateFilePopup();

    // TODO: status bar
    ImGui::End();
}

void ContentBrowserPanel::DisplayFileHierarchy(const std::filesystem::path &directory)
{
    // TODO: sync with the file system so that they open the same directory
    namespace fs = std::filesystem;
    if (!fs::exists(directory) || !fs::is_directory(directory)) return;

    for (auto &directoryEntry : std::filesystem::directory_iterator(directory))
    {
        const fs::path &entryPath = directoryEntry.path();

		if (entryPath.stem() == "AssetRegistry") continue;

        // Get the hash of the node label
        std::size_t labelHash = std::hash<std::string>{}(entryPath.filename().string());
        bool &isOpen = m_NodeOpenStatusMap[labelHash];

        // Check if the entry is a directory
        if (fs::is_directory(entryPath))
        {
            auto path = entryPath.filename().string();
            auto flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;
            bool treeNodeOpen = ImGui::TreeNodeEx(
                path.c_str(), flags, !isOpen ? ICON_FA_FOLDER "  %s" : ICON_FA_FOLDER_OPEN "  %s", path.c_str());
            // if (entryPath == m_CurrentDirectory) treeNodeOpen = true;

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
                    AssetManager::ImportAsset(relativePath);
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
        static char name[128] = "New File\0";
        ImGui::Text(ICON_FA_PHOTO_VIDEO "   Create File");
        ImGui::Separator();
        ImGui::InputText(" ", name, IM_ARRAYSIZE(name));
        ImGui::SameLine();
        if (ImGui::Button(ICON_FA_CHECK "  Save", ImVec2(100, 0)))
        {
            auto path = m_CurrentDirectory / name;
            std::string extension = m_NewAssetType == AssetType::Scene      ? ".scene"
                                    : m_NewAssetType == AssetType::Material ? ".material"
                                                                            : "";
            std::ofstream file(path.string() + extension);
            AssetManager::ImportAsset(path.string() + extension);
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

void ContentBrowserPanel::Search(const std::string &query) 
{
    m_CurrentDirectoryEntries.clear();
	
	if (query.empty()) return;

	// convert query to lowercase
	std::string lowerQuery = query;
	std::transform(lowerQuery.begin(), lowerQuery.end(), lowerQuery.begin(), [](unsigned char c) { return std::tolower(c); });

	for (auto& p : std::filesystem::recursive_directory_iterator(m_BaseDirectory))
	{
		auto fileName = p.path().filename().string();
		std::transform(fileName.begin(), fileName.end(), fileName.begin(), [](unsigned char c) { return std::tolower(c); });
		if (fileName.find(lowerQuery) != std::string::npos)
		{
			m_CurrentDirectoryEntries.push_back(p);
		}
	}
}

void ContentBrowserPanel::DrawFileAssetBrowser(std::filesystem::directory_entry directoryEntry) 
{
    static float padding = 50.0f;
    static float thumbnailSize = 160.0f;
    float cellSize = thumbnailSize + padding;

    float panelWidth = ImGui::GetContentRegionAvail().x;
    float dirTreeWidth = panelWidth * 0.18;
    int columnCount = (int)(panelWidth / cellSize);
    if (columnCount < 1) columnCount = 1;

    const auto &path = directoryEntry.path();
    std::string filenameString = path.filename().string();

    auto relativePath = std::filesystem::relative(path, Project::GetAssetDirectory());

    if (path.stem() == "AssetRegistry") return;

    ImGui::PushID(filenameString.c_str());
    Texture2DRef icon = directoryEntry.is_directory() ? m_DirectoryIcon : m_FileIcon;
    if (path.extension() == ".scene") icon = sceneIcon;

    /*if (path.extension() == ".png" || path.extension() == ".jpg" || path.extension() == ".jpeg")
		icon = AssetManager::GetAsset<Texture2D>(relativePath);*/

    // only display Source folder of Scripts
    if (m_CurrentDirectory == Project::GetAssetDirectory() / "Scripts")
    {
        if (path.filename() != "Source")
        {
            ImGui::PopID();
            return;
        }
    }

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
    ImGui::ImageButton((void *)(intptr_t)icon->GetRendererID(), {thumbnailSize, thumbnailSize}, {0, 1}, {1, 0});
    ImGui::PopStyleColor();

    if (!directoryEntry.is_directory() && ImGui::BeginPopupContextItem())
    {
        if (ImGui::MenuItem(ICON_FA_TRASH "   Delete"))
        {
            // AssetManager::UnloadAsset(std::stoull(assetHandle));
            std::filesystem::remove(path);
        }
        ImGui::EndPopup();
    }

    if (ImGui::BeginDragDropSource())
    {
        auto assetHandle = AssetManager::GetAssetHandleFromPath(relativePath).ToString();
        ImGui::SetDragDropPayload("CONTENT_BROWSER_ITEM", assetHandle.c_str(),
                                    (strlen(assetHandle.c_str()) + 1) * sizeof(char *));
        ImGui::Button(relativePath.string().c_str());
        ImGui::EndDragDropSource();
    }

    if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
    {
		m_CurrentDirectoryEntries.clear();
		strcpy(searchStr, "");

        if (directoryEntry.is_directory()) m_CurrentDirectory /= path.filename();
        else
        {
            if (path.extension() == ".material")
            {
				auto materialHandle = AssetManager::GetAssetHandleFromPath(relativePath);
				MaterialEditorPanel::OpenMaterialEditor(materialHandle);
            }
        }
    }

    ImGui::TextWrapped(filenameString.c_str());
    ImGui::NextColumn();

    ImGui::PopID();
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
