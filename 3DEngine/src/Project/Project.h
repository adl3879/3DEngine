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
    std::filesystem::path AssetRegistryPath = "AssetRegistry.yaml";
};

class Project
{
public:
    Project() = default;

    static const std::filesystem::path &GetProjectDirectory() { return s_ActiveProject->m_ProjectDirectory; }

    static std::filesystem::path GetAssetDirectory()
    {
        return GetProjectDirectory() / s_ActiveProject->m_Config.AssetDirectory;
    }

    static std::filesystem::path GetAssetRegistryPath()
    {
        return GetAssetDirectory() / s_ActiveProject->m_Config.AssetRegistryPath;
    }

    static std::filesystem::path GetScriptModulePath()
    {
        return GetAssetDirectory() / s_ActiveProject->m_Config.ScriptModulePath;
    }

    static std::string GetProjectName() { return s_ActiveProject->m_Config.Name; }

    ProjectConfig &GetConfig() { return m_Config; }
    void SetConfig(const ProjectConfig &config) { m_Config = config; }

    static std::shared_ptr<Project> GetActive() { return s_ActiveProject; }
    std::shared_ptr<AssetManagerBase> GetAssetManager() { return m_AssetManager; }

    std::shared_ptr<EditorAssetManager> GetEditorAssetManager()
    {
        return std::static_pointer_cast<EditorAssetManager>(m_AssetManager);
    }

	static std::filesystem::path GetImportCachePath() { return GetProjectDirectory() / "Library\\Imported"; }

	static std::filesystem::path GetProjectConfigPath() { return s_ProjectConfigPath; }

    static std::shared_ptr<Project> New();
    static std::shared_ptr<Project> New(const std::filesystem::path &path);
    static std::shared_ptr<Project> Load(const std::filesystem::path &path);
    static bool SaveActive(const std::filesystem::path &path);

private:
    ProjectConfig m_Config;
    std::filesystem::path m_ProjectDirectory;
    std::shared_ptr<AssetManagerBase> m_AssetManager;

    inline static std::filesystem::path s_ProjectConfigPath;
    inline static std::shared_ptr<Project> s_ActiveProject;
};

using ProjectRef = std::shared_ptr<Project>;
} // namespace Engine
