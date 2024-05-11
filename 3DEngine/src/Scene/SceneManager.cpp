#include "SceneManager.h"

#include "SceneSerializer.h"
#include "FileDialogs.h"
#include "AssetManager.h"
#include "Components.h"

namespace Engine
{
SceneManager &SceneManager::Get()
{
    static SceneManager instance;
	return instance;
}

SceneManager::SceneManager() 
{ 
	m_EditorScene = std::make_shared<Scene>();
	m_ActiveScene = m_EditorScene;
}

void SceneManager::NewScene()
{
    m_ActiveScene = std::make_unique<Scene>();
}

void SceneManager::OpenScene()
{
    // Fix later
    auto path = FileDialogs::OpenFile("3D Engine Scene (*.scene)\0*.scene\0");
    if (!path.empty())
    {
        auto handle = AssetManager::ImportAsset(path);
        m_EditorScene = AssetManager::GetAsset<Scene>(handle);

        m_ActiveScene = m_EditorScene;
    }
}

void SceneManager::SaveSceneAs()
{
    auto path = FileDialogs::SaveFile("3D Engine Scene (*.scene)\0*.scene\0");
    if (!path.empty())
    {
        m_ActiveScene->SetSceneFilePath(path);
        SceneSerializer serializer(m_ActiveScene);
        serializer.Serialize(path);
    }
}

void SceneManager::SaveScene()
{
    if (m_ActiveScene->GetSceneFilePath().empty()) SaveSceneAs();
    else
    {
        SceneSerializer serializer(m_ActiveScene);
        serializer.Serialize(m_ActiveScene->GetSceneFilePath());
    }
}

void SceneManager::OnScenePlay()
{
    m_SceneState = SceneState::Play;
    m_ActiveScene = Scene::Copy(m_EditorScene);

    m_ActiveScene->SetPlaying(true);
    m_ActiveScene->OnRuntimeStart();;
}

void SceneManager::OnSceneStop()
{
    // TODO: fix screen jittering when stopping
    m_SceneState = SceneState::Edit;
    m_ActiveScene = m_EditorScene;

    m_ActiveScene->SetPlaying(false);
    m_ActiveScene->OnRuntimeStop();
}

void SceneManager::SetActiveScene(const SceneRef &scene)
{
	m_ActiveScene = scene;
}

void SceneManager::ShowMainScene()
{
    m_ActiveScene->SetSceneType(SceneType::Scene3D);
    for (auto e : m_ActiveScene->GetRegistry().view<TagComponent, VisibilityComponent, ParentComponent, IDComponent>())
    {
        auto entity = Entity{e, m_ActiveScene.get()};
        entity.GetComponent<VisibilityComponent>().IsVisible = true;

    }

    for (auto e : m_ActiveScene->GetRegistry().view<TransformComponent, PrevTransformComponent>())
    {
		auto entity = Entity{e, m_ActiveScene.get()};
        auto &transform = entity.GetComponent<TransformComponent>();
        auto &prevTransform = entity.GetComponent<PrevTransformComponent>();
        transform.Translation = prevTransform.Translation;
        transform.Rotation = prevTransform.Rotation;
        entity.RemoveComponent<PrevTransformComponent>();
    }
}
} // namespace Engine
