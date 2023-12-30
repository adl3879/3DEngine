#include "ContentBrowserPanel.h"

#include <imgui.h>

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

Texture2DRef sceneIcon, backIcon, forwardIcon, prefabIcon, cSharpIcon;

glm::vec2 thumbnailSize = {130.0f, 120.0f};

namespace Utils
{
} // namespace Utils

ContentBrowserPanel::ContentBrowserPanel()
{
    m_DirectoryIcon = TextureImporter::LoadTexture2D("Resources/Icons/ContentBrowser/DirectoryIcon.png");
    m_FileIcon = TextureImporter::LoadTexture2D("Resources/Icons/ContentBrowser/FileIcon.png");
    sceneIcon = TextureImporter::LoadTexture2D("Resources/Icons/ContentBrowser/SceneIcon.png");
    backIcon = TextureImporter::LoadTexture2D("Resources/Icons/ContentBrowser/BackIcon.png");
    forwardIcon = TextureImporter::LoadTexture2D("Resources/Icons/ContentBrowser/ForwardIcon.png");
    prefabIcon = TextureImporter::LoadTexture2D("Resources/Icons/ContentBrowser/PrefabIcon.png");
    cSharpIcon = TextureImporter::LoadTexture2D("Resources/Icons/ContentBrowser/CSharpIcon.png");

    m_ThumbnailCache = std::make_shared<ThumbnailCache>();

    m_Mode = Mode::FileSystem;
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
                const auto path = FileDialogs::OpenFile("3D Model (*.fbx *.obj *.gltf)\0*.fbx;*.obj;*.gltf\0");
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

    CreateFilePopup();
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

            // change selection color
            // ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0.925f, 0.75f, 0.4666f, 0.6f));
            // ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(0.925f, 0.75f, 0.4666f, 0.6f));
            // ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.925f, 0.75f, 0.4666f, 0.6f));

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

void ContentBrowserPanel::CreateFilePopup()
{
    if (m_OpenCreateFilePopup)
    {
        ImGui::OpenPopup("create_scene_file");
        m_OpenCreateFilePopup = false;
    }
    if (ImGui::BeginPopup("create_scene_file"))
    {
        static char name[128] = "\0";

        ImGui::Text("Save As");
        ImGui::InputText(" ", name, IM_ARRAYSIZE(name));

        ImGui::SameLine();
        if (m_SaveRequested)
        {
            if (m_NewAssetType == AssetType::Prefab)
            {
                const auto prefab = std::make_shared<Prefab>();
                prefab->CreateFromEntity(m_PrefabDraggedEntity);
                PrefabSerializer serializer(prefab);
                serializer.Serialize(m_CurrentDirectory / (std::string(name) + ".prefab"));
            }
            else if (m_NewAssetType == AssetType::Folder)
            {
                std::filesystem::create_directory(m_CurrentDirectory / name);
            }
            else
            {
                const auto path = m_CurrentDirectory / name;
                const auto relativePath = std::filesystem::relative(path, Project::GetAssetDirectory());
                const std::string extension = m_NewAssetType == AssetType::Scene       ? ".scene"
                                              : m_NewAssetType == AssetType::Material  ? ".material"
                                              : m_NewAssetType == AssetType::Shader    ? ".shader"
                                              : m_NewAssetType == AssetType::NetScript ? ".cs"
                                                                                       : "";
                std::ofstream file(path.string() + extension);
                AssetManager::ImportAsset(relativePath.string() + extension);
                file.close();
            }
            m_SaveRequested = false;
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }

    // Handle Enter key press
    if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Enter), false))
    {
        m_SaveRequested = true;
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
    //    static float thumbnailSize = 120.0f + 15;

    const auto &path = directoryEntry.path();
    const std::string filenameString = path.filename().string();

    auto relativePath = std::filesystem::relative(path, Project::GetAssetDirectory());

    if (path.stem() == "AssetRegistry") return;

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

    if (path.extension() == ".prefab") icon = prefabIcon;

    if (path.extension() == ".cs") icon = cSharpIcon;

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
    ImGui::ImageButton((void *)(intptr_t)icon->GetRendererID(), {thumbnailSize.x, thumbnailSize.y}, {0, 1}, {1, 0}, 15);
    ImGui::PopStyleColor();

    if (!directoryEntry.is_directory() && ImGui::BeginPopupContextItem())
    {
        if (ImGui::MenuItem(ICON_FA_TRASH "   Delete"))
        {
            std::filesystem::remove(path);
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

    std::string truncatedName = filenameString.substr(0, filenameString.find_last_of("."));
    if (truncatedName.size() > 10) truncatedName = truncatedName.substr(0, 10) + "...";

    // center
    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (thumbnailSize.x - ImGui::CalcTextSize(truncatedName.c_str()).x) / 2);
    ImGui::Text("%s", truncatedName.c_str());
    if (ImGui::IsItemHovered())
    {
        ImGui::BeginTooltip();

        ImGui::Text("%s", filenameString.c_str());
        ImGui::EndTooltip();
    }

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
