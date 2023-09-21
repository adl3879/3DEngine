#pragma once

#include <filesystem>
#include <vector>
#include <map>
#include <unordered_map>

#include "Texture2D.h"

namespace Engine
{
class ContentBrowserPanel
{
  public:
    ContentBrowserPanel();

    void OnImGuiRender();

  private:
    // TreeNode structure
    struct FileTreeNode
    {
        std::map<std::string, FileTreeNode> Children;
    };
    FileTreeNode m_FileTree;
    FileTreeNode *currentNode = &m_FileTree;
    std::vector<FileTreeNode *> m_NodeStack; // Stack to keep track of visited nodes

  private:
    void AddPathToTree(FileTreeNode &root, const std::filesystem::path &path);
    void RefreshAssetTree();
    void DisplayFileHierarchy(const std::filesystem::path &directory);
    void CreateFilePopup();
    void OpenCreateFilePopup(AssetType type);

	// Search
	void Search(const std::string& query);
	void DrawFileAssetBrowser(std::filesystem::directory_entry directoryEntry);

  private:
    std::filesystem::path m_BaseDirectory;
    std::filesystem::path m_CurrentDirectory, m_AssetCurrentDirectory;
    std::unordered_map<std::size_t, bool> m_NodeOpenStatusMap;
	std::vector<std::filesystem::path> m_DirectoryStack;
	std::vector<std::filesystem::directory_entry> m_CurrentDirectoryEntries;

    Texture2DRef m_DirectoryIcon, m_FileIcon;
    std::map<std::string, AssetHandle> m_AssetHandles;

    AssetType m_NewAssetType = AssetType::None;
    bool m_OpenCreateFilePopup;

    enum class Mode
    {
        Asset = 0,
        FileSystem = 1
    };

    Mode m_Mode = Mode::Asset;
    std::string m_SearchQuery;
};
} // namespace Engine
