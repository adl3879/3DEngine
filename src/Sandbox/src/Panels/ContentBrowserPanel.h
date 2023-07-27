#pragma once

#include <filesystem>

namespace Engine
{
class ContentBrowserPanel
{
  public:
    ContentBrowserPanel();

    void OnImGuiRender();

  private:
    std::filesystem::path m_BaseDirectory;
    std::filesystem::path m_CurrentDirectory;

    unsigned int m_DirectoryIcon, m_FileIcon;
};
}