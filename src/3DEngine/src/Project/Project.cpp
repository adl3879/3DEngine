#include "Project.h"

#include "ProjectSerializer.h"
#include "Log.h"
#include <iostream>

namespace Engine
{
std::shared_ptr<Project> Project::New()
{
    s_ActiveProject = std::make_shared<Project>();
    return s_ActiveProject;
}

std::shared_ptr<Project> Project::New(const std::filesystem::path &path)
{
    // create empty folders
    std::filesystem::create_directory(path / "Assets");
    std::filesystem::create_directory(path / "Assets/Textures");
    std::filesystem::create_directory(path / "Assets/Models");
    std::filesystem::create_directory(path / "Assets/Scripts");
    std::filesystem::create_directory(path / "Assets/Scenes");
    std::filesystem::create_directory(path / "Assets/Materials");

    s_ActiveProject = std::make_shared<Project>();
    s_ActiveProject->m_ProjectDirectory = path;
    ProjectConfig config = ProjectConfig{
        .Name = path.filename().string(),
        .AssetDirectory = "Assets",
        .AssetRegistryPath = "Assets/AssetRegistry.json",
    };
    s_ActiveProject->SetConfig(config);
    auto fileName = path.filename().string() + ".3dproj";
    SaveActive(path / fileName);

    return s_ActiveProject;
}

std::shared_ptr<Project> Project::Load(const std::filesystem::path &path)
{
    std::shared_ptr<Project> project = std::make_shared<Project>();

    ProjectSerializer serializer(project);
    if (serializer.Deserialize(path))
    {
        project->m_ProjectDirectory = path.parent_path();
        s_ActiveProject = project;
        LOG_CORE_INFO("Project {} opened", project->GetConfig().Name);
        return s_ActiveProject;
    }
    return nullptr;
}

bool Project::SaveActive(const std::filesystem::path &path)
{
    ProjectSerializer serializer(s_ActiveProject);
    if (serializer.Serialize(path))
    {
        std::cout << "PATH: " << path.parent_path().string() << std::endl;
        s_ActiveProject->m_ProjectDirectory = path.parent_path();
        return true;
    }

    return false;
}
} // namespace Engine