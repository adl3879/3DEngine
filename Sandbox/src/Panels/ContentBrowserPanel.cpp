#include "ContentBrowserPanel.h"

#include <imgui.h>
#include <FileWatch.h>

#include "Project.h"
#include "TextureImporter.h"
#include "AssetManager.h"
#include "Log.h"
#include "IconsFontAwesome5.h"
#include "MaterialEditorPanel.h"
#include "Utils/FileDialogs.h"
#include "Misc/ThumbnailManager.h"
#include "MeshImporter.h"
#include "Prefab.h"
#include "PrefabSerializer.h"

#include <iostream>
#include <fstream>
#include <cctype>

namespace Engine
{
bool openCreateFilePopup = false;

static char searchStr[128] = "";

Texture2DRef sceneIcon, backIcon, forwardIcon, prefabIcon, cSharpIcon, modelIcon;

glm::vec2 thumbnailSize = {100.0f, 100.0f};

namespace Utils
{
} // namespace Utils

struct ContentBrowserData
{
    std::unique_ptr<filewatch::FileWatch<std::string>> RenameWatcher;
	std::string CurrentRenamePath;
};
ContentBrowserData *s_Data = new ContentBrowserData();

ContentBrowserPanel::ContentBrowserPanel()
{
    m_DirectoryIcon = TextureImporter::LoadTexture2D("Resources/Icons/ContentBrowser/DirectoryIcon.png");
    m_FileIcon = TextureImporter::LoadTexture2D("Resources/Icons/ContentBrowser/FileIcon.png");
    sceneIcon = TextureImporter::LoadTexture2D("Resources/Icons/ContentBrowser/SceneIcon.png");
    backIcon = TextureImporter::LoadTexture2D("Resources/Icons/ContentBrowser/BackIcon.png");
    forwardIcon = TextureImporter::LoadTexture2D("Resources/Icons/ContentBrowser/ForwardIcon.png");
    prefabIcon = TextureImporter::LoadTexture2D("Resources/Icons/ContentBrowser/PrefabIcon.png");
    cSharpIcon = TextureImporter::LoadTexture2D("Resources/Icons/ContentBrowser/CSharpIcon.png");
    modelIcon = TextureImporter::LoadTexture2D("Resources/Icons/ContentBrowser/3DModelIcon.png");

    m_ThumbnailCache = std::make_shared<ThumbnailCache>();

	s_Data->RenameWatcher = std::make_unique<filewatch::FileWatch<std::string>>(
		"C:\\dev\\3DEngine\\Sandbox\\SandboxProject\\Assets",
		[](const std::string &path, const filewatch::Event change_type)
		{
			if (change_type == filewatch::Event::renamed_new)
			{
                if (Utils::GetAssetTypeFromExtension(path) == AssetType::Mesh)
                {
                    LOG_CORE_CRITICAL("File renamed from {0} to {1}", s_Data->CurrentRenamePath,  path);
					if (MeshImporter::LoadModel(Project::GetAssetDirectory() / path, ""))
					{
						// delete import file
						auto importFile = s_Data->CurrentRenamePath + ".import";
						std::filesystem::remove(importFile);
					}
                }
			}
		});
}

void ContentBrowserPanel::OnImGuiRender()
{
    ImGui::Begin("Content Browser");

    float panelWidth = ImGui::GetContentRegionAvail().x;
    float dirTreeWidth = panelWidth * 0.17;

    static float padding = 80.0f;
    const float cellSize = thumbnailSize.x + padding;

    int columnCount = static_cast<int>(panelWidth / cellSize);
    if (columnCount < 1) columnCount = 1;

    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.05f, 0.05f, 0.05f, 0.54f));
    ImGui::BeginChild("Directory Tree", ImVec2(dirTreeWidth, 0), true, ImGuiWindowFlags_HorizontalScrollbar);
    if (ImGui::TreeNodeEx(ICON_FA_HOME "  Root Directory", ImGuiTreeNodeFlags_DefaultOpen))
    {
        if (ImGui::IsItemClicked())
        {
            m_CurrentDirectory = m_BaseDirectory;
            m_NodeStack.clear();
        }
        DisplayFileHierarchy(m_BaseDirectory);
        ImGui::TreePop();
    }

    ImGui::EndChild();
    ImGui::PopStyleColor();

    ImGui::SameLine();

    ImGui::BeginChild("Content Region", ImVec2(panelWidth - dirTreeWidth, 0), false);

    ImGui::Begin("##content_browser_toolbar", nullptr,
                 ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoScrollbar);

    ImGui::Text("Content Browser");
    ImGui::SameLine();
    ImGui::TextColored({0.5f, 0.5f, 0.5f, 1.0f}, "%s", m_CurrentDirectory.string().c_str());

    ImGui::SameLine();

    // volume slider to control thumbnail size
    // float to right
    ImGui::SetCursorPosX(ImGui::GetWindowWidth() - 150);

    ImGui::SliderFloat("##thumbnail_size", &thumbnailSize.x, 100, 512);
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
    ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.05f, 0.05f, 0.05f, 0.54f));
    if (ImGui::InputTextWithHint("##Search", ICON_FA_SEARCH "  Search", searchStr, IM_ARRAYSIZE(searchStr)))
    {
        Search(searchStr);
    }
    ImGui::PopStyleColor();
    ImGui::PopStyleVar(2);

    ImGui::PopItemWidth();

    ImGui::SameLine();
    ImGui::SetCursorPosX(ImGui::GetWindowWidth() - 30);

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.2, 0.2, 0.2, 0.2));
    if (ImGui::Button(ICON_FA_COG))
    {
    }
    ImGui::PopStyleColor(2);

    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10);
    ImGui::Separator();
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10);

    ImGui::Columns(columnCount, nullptr, false);

    // Right-click on blank space
    if (ImGui::BeginPopupContextWindow())
    {
        if (ImGui::BeginMenu(ICON_FA_PLUS "  Add Item"))
        {
            if (ImGui::MenuItem(ICON_FA_FOLDER "   New Folder")) OpenCreateFilePopup(AssetType::Folder);
            if (ImGui::MenuItem(ICON_FA_FILE "   New File")) OpenCreateFilePopup(AssetType::None);
            ImGui::Separator();
            if (ImGui::MenuItem(ICON_FA_PHOTO_VIDEO "   Scene")) OpenCreateFilePopup(AssetType::Scene);
            if (ImGui::MenuItem(ICON_FA_PAINT_BRUSH "   Material")) OpenCreateFilePopup(AssetType::Material);
            if (ImGui::MenuItem(ICON_FA_FILE_CODE "   Shader")) OpenCreateFilePopup(AssetType::Shader);
            ImGui::EndPopup();
        }
        if (ImGui::BeginMenu(ICON_FA_FILE_IMPORT "   Import"))
        {
            if (ImGui::MenuItem(ICON_FA_IMAGE "   Texture"))
            {
                // load texture file from path and copy to asset directory
                const auto path = FileDialogs::OpenFile("Image (*.png *.jpg *.jpeg)\0*.png;*.jpg;*.jpeg\0");
                std::filesystem::copy(path, m_CurrentDirectory / std::filesystem::path(path).filename());
            }
            if (ImGui::MenuItem(ICON_FA_CUBE "   3D Mesh"))
            {
                const auto path = FileDialogs::OpenFile("3D Model (*.fbx *.dae *.gltf)\0*.fbx;*.dae;*.gltf\0");
                MeshImporter::LoadModel(path, m_CurrentDirectory);
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
        for (const auto &directoryEntry : m_CurrentDirectoryEntries) DrawFileAssetBrowser(directoryEntry);
    }

    ImGui::Columns(1);
    ImGui::EndChild();

    if (ImGui::BeginDragDropTarget())
    {
        if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("ENTITY"))
        {
            m_PrefabDraggedEntity = *static_cast<const Entity *>(payload->Data);
            OpenCreateFilePopup(AssetType::Prefab);
        }
        ImGui::EndDragDropTarget();
    }

    // TODO: status bar
    ImGui::End();

    m_Context->RenderTextEditors();
    m_ThumbnailCache->OnUpdate();
    ThumbnailManager::Get().OnUpdate();
}

void ContentBrowserPanel::DisplayFileHierarchy(const std::filesystem::path &directory)
{
    namespace fs = std::filesystem;
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
            auto path = entryPath.filename().string();
            int flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;

            // check if entryPath is a child of m_CurrentDirectory
            if (m_CurrentDirectory == entryPath)
            {
                flags |= ImGuiTreeNodeFlags_Selected;
            }

            // check if entryPath is a leaf folder
            bool isLeaf = true;
            for (const auto &p : fs::recursive_directory_iterator(entryPath))
            {
                if (p.is_directory())
                {
                    isLeaf = false;
                    break;
                }
            }
            if (isLeaf) flags |= ImGuiTreeNodeFlags_Leaf;

            const bool treeNodeOpen =
                ImGui::TreeNodeEx(path.c_str(), flags,
                                  !isOpen || isLeaf ? ICON_FA_FOLDER "  %s" : ICON_FA_FOLDER_OPEN "  %s", path.c_str());

            // ImGui::PopStyleColor(3);

            // if treenode is selected
            if (ImGui::IsItemClicked()) m_CurrentDirectory = entryPath;

            if (treeNodeOpen)
            {
                isOpen = true;
                DisplayFileHierarchy(entryPath);
                ImGui::TreePop();
            }
            else
                isOpen = false;
        }
    }
}

void ContentBrowserPanel::OpenCreateFilePopup(AssetType type)
{
	if (type == AssetType::Folder)
	{
		std::filesystem::create_directory(m_CurrentDirectory / "New Folder");
		m_RenameRequested = true;
		m_RenamePath = m_CurrentDirectory / "New Folder";
	}
	else
	{
		std::string defaultName = type == AssetType::Scene		? "Scene.scene"
					   : type == AssetType::Material	? "New Material.material"
					   : type == AssetType::Shader		? "New Shader.shader"
					   : type == AssetType::NetScript	? "New Script.cs"
                       : type == AssetType::Prefab		? "New Prefab.prefab"
														: "New File.txt";

		std::ofstream file(m_CurrentDirectory / defaultName);
		file.close();
		m_RenameRequested = true;
		m_RenamePath = m_CurrentDirectory / defaultName;

		if (type == AssetType::Prefab)
		{
			auto prefab = std::make_shared<Prefab>();
			prefab->CreateFromEntity(m_PrefabDraggedEntity);
			PrefabSerializer serializer(prefab);
			serializer.Serialize(m_CurrentDirectory / defaultName);
		}
	}
}

void ContentBrowserPanel::Search(const std::string &query)
{
    m_CurrentDirectoryEntries.clear();

    if (query.empty()) return;

    // convert query to lowercase
    std::string lowerQuery = query;
    std::transform(lowerQuery.begin(), lowerQuery.end(), lowerQuery.begin(),
                   [](unsigned char c) { return std::tolower(c); });

    for (auto &p : std::filesystem::recursive_directory_iterator(m_BaseDirectory))
    {
        auto fileName = p.path().filename().string();
        std::transform(fileName.begin(), fileName.end(), fileName.begin(),
                       [](unsigned char c) { return std::tolower(c); });
        if (fileName.find(lowerQuery) != std::string::npos)
        {
            m_CurrentDirectoryEntries.push_back(p);
        }
    }
}

void ContentBrowserPanel::DrawFileAssetBrowser(std::filesystem::directory_entry directoryEntry)
{
    const auto &path = directoryEntry.path();
    const std::string filenameString = path.filename().string();

    auto relativePath = std::filesystem::relative(path, Project::GetAssetDirectory());

    if (path.stem() == "AssetRegistry") return;
    if (path.extension() == ".import") return;
	if (path.extension() == ".bin") return;

    ImGui::PushID(filenameString.c_str());
    Texture2DRef icon = directoryEntry.is_directory() ? m_DirectoryIcon : m_FileIcon;
    if (path.extension() == ".scene") icon = sceneIcon;

    if (path.extension() == ".png" || path.extension() == ".jpg" || path.extension() == ".jpeg")
    {
        icon = m_ThumbnailCache->GetOrCreateThumbnail(relativePath);
        if (!icon) icon = m_FileIcon;
    }

    if (path.extension() == ".material")
    {
        icon = ThumbnailManager::Get().GetThumbnail(relativePath);
        if (!icon) icon = m_FileIcon;
    }
    if (path.extension() == ".gltf" || path.extension() == ".fbx" || path.extension() == ".dae")
	{
		icon = modelIcon;
	}

    if (path.extension() == ".prefab") icon = prefabIcon;

    if (path.extension() == ".cs") icon = cSharpIcon;

	ImGui::BeginGroup();
	ImGui::PushID(filenameString.c_str());

	auto scrPos = ImGui::GetCursorScreenPos();
    int thumbnailPadding = 20;

	// Draw background color
    /*ImGui::GetWindowDrawList()->AddRectFilled(
        scrPos,
        ImVec2(ImGui::GetCursorScreenPos().x + thumbnailSize.x + (thumbnailPadding * 2), ImGui::GetCursorScreenPos().y + 200),
        IM_COL32(0.05 * 255, 0.05 * 255, 0.05 * 255, 0.54 * 255), 0);*/

	// change button color
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.2, 0.2, 0.2, 0.2));
	ImGui::ImageButton((void *)(intptr_t)icon->GetRendererID(), {thumbnailSize.x, thumbnailSize.y}, {0, 1}, {1, 0}, thumbnailPadding);
	ImGui::PopStyleColor(2);

	if (ImGui::BeginPopupContextItem())
    {
        if (ImGui::MenuItem(ICON_FA_TRASH "   Delete"))
        {
            std::filesystem::remove(path);
        }
        // Rename
		if (ImGui::MenuItem(ICON_FA_PEN "   Rename"))
		{
			m_RenameRequested = true;
			m_RenamePath = path;
		}
		ImGui::EndPopup();
	}

    if (ImGui::BeginDragDropSource())
    {
        ImGui::SetDragDropPayload("CONTENT_BROWSER_ITEM", relativePath.string().c_str(),
                                  (strlen(relativePath.string().c_str()) + 1) * sizeof(char *));
        ImGui::Button(relativePath.string().c_str());
        ImGui::EndDragDropSource();
    }

    if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
    {
        m_CurrentDirectoryEntries.clear();
        strcpy(searchStr, "");

        if (directoryEntry.is_directory())
            m_CurrentDirectory /= path.filename();
        else
        {
            if (path.extension() == ".material")
            {
                auto materialHandle = AssetManager::GetAssetHandleFromPath(relativePath);
                MaterialEditorPanel::OpenMaterialEditor(materialHandle);
            }
            else if (path.extension() == ".lua" || path.extension() == ".shader")
            {
                m_Context->InitTextEditor(path);
            }
        }
    }

    std::string fileNameWithoutExtension = filenameString.substr(0, filenameString.find_last_of("."));
	std::string truncatedName = fileNameWithoutExtension;
    if (truncatedName.size() > 10) truncatedName = truncatedName.substr(0, 10) + "...";

    // center
    auto cps = ImGui::GetCursorPosX();
    auto ps = ImGui::GetCursorPosX() +
              (thumbnailSize.x + (thumbnailPadding * 2) - ImGui::CalcTextSize(truncatedName.c_str()).x) / 2;
    ImGui::SetCursorPosX(ps);
    //ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10);


    if (m_RenameRequested && m_RenamePath == path)
    {
        ImGui::SetKeyboardFocusHere();

        static char name[128] = "\0";
        strcpy(name, fileNameWithoutExtension.c_str());
		ImGui::SetCursorPosX(cps + thumbnailPadding);
        ImGui::PushItemWidth(thumbnailSize.x);
		ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
		// text color
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
		// align text to center
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
		ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 2.0f);
        ImGui::InputText("##rename", name, IM_ARRAYSIZE(name));
		ImGui::PopStyleVar(2);
		ImGui::PopStyleColor(2);
        ImGui::PopItemWidth();
        if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Enter), false))
        {
            if (strlen(name) >= 2)
            {
				s_Data->CurrentRenamePath = path.string();
                m_RenameRequested = false;
                m_RenamePath = "";
                auto extension = path.extension();
                auto newName = extension.empty() ? name : name + extension.string();
                std::filesystem::rename(path, path.parent_path() / newName);
            }
        }
    }
    else
    {
        ImGui::TextWrapped("%s", truncatedName.c_str());
		if (ImGui::IsItemHovered())
        {
			ImGui::BeginTooltip();
			ImGui::Text("%s", filenameString.c_str());
			ImGui::EndTooltip();
		}
    }

	ImGui::PopID();
    ImGui::EndGroup();

	//ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 30);

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

void ContentBrowserPanel::SetContext(const SceneRef &context)
{
    m_Context = context;
    m_BaseDirectory = Project::GetAssetDirectory();
    m_CurrentDirectory = m_BaseDirectory;
    RefreshAssetTree();
}

} // namespace Engine
