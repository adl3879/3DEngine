#pragma once

#include <filesystem>
#include <vector>
#include <map>

#include "Texture.h"

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

  private:
    std::filesystem::path m_BaseDirectory;
    std::filesystem::path m_CurrentDirectory;

    Texture2DRef m_DirectoryIcon, m_FileIcon;

    enum class Mode
    {
        Asset = 0,
        FileSystem = 1
    };

    Mode m_Mode = Mode::Asset;
};
} // namespace Engine