#include "Project.h"

#include "ProjectSerializer.h"
#include "Log.h"
#include <iostream>

namespace Engine
{
std::shared_ptr<Project> Project::New()
{
    s_ActiveProject = std::make_shared<Project>();
    std::shared_ptr<EditorAssetManager> editorAssetManager = std::make_shared<EditorAssetManager>();
    s_ActiveProject->m_AssetManager = editorAssetManager;
    editorAssetManager->DeserializeAssetRegistry();
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

    s_ActiveProject = New();
    s_ActiveProject->m_ProjectDirectory = path;
    ProjectConfig config = ProjectConfig{
        .Name = path.filename().string(),
        .AssetDirectory = "Assets",
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
        s_ActiveProject = project;
        s_ActiveProject->m_ProjectDirectory = path.parent_path();

        std::shared_ptr<EditorAssetManager> editorAssetManager = std::make_shared<EditorAssetManager>();
        s_ActiveProject->m_AssetManager = editorAssetManager;
        editorAssetManager->DeserializeAssetRegistry();

        LOG_CORE_INFO("Project {} opened", s_ActiveProject->m_ProjectDirectory.string());
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