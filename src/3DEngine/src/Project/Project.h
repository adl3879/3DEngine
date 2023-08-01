#pragma once

#include <string>
#include <filesystem>
#include <memory>

#include "EditorAssetManager.h"

namespace Engine
{
struct ProjectConfig
{
    std::string Name = "Untitled";

    std::filesystem::path StartScene;

    std::filesystem::path AssetDirectory;
    std::filesystem::path ScriptModulePath;
    std::filesystem::path AssetRegistryPath;
};

class Project
{
  public:
    static const std::filesystem::path &GetProjectDirectory() { return s_ActiveProject->m_ProjectDirectory; }

    static std::filesystem::path GetAssetDirectory()
    {
        return GetProjectDirectory() / s_ActiveProject->m_Config.AssetDirectory;
    }

    static std::filesystem::path GetAssetRegistryPath()
    {
        return GetAssetDirectory() / s_ActiveProject->m_Config.AssetRegistryPath;
    }

    ProjectConfig &GetConfig() { return m_Config; }

    static std::shared_ptr<Project> GetActive() { return s_ActiveProject; }
    std::shared_ptr<AssetManagerBase> GetAssetManager() { return m_AssetManager; }
    std::shared_ptr<EditorAssetManager> GetEditorAssetManager()
    {
        return std::static_pointer_cast<EditorAssetManager>(m_AssetManager);
    }

    static std::shared_ptr<Project> New();
    static std::shared_ptr<Project> Load(const std::filesystem::path &path);
    static bool SaveActive(const std::filesystem::path &path);

  private:
    ProjectConfig m_Config;
    std::filesystem::path m_ProjectDirectory;
    std::shared_ptr<AssetManagerBase> m_AssetManager;

    inline static std::shared_ptr<Project> s_ActiveProject;
};

using ProjectPtr = std::shared_ptr<Project>;
} // namespace Engine