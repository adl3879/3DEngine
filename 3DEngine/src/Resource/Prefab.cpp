#include "Prefab.h"

#include "PrefabSerializer.h"
#include "AssetManager.h"
#include "Components.h"
#include "SceneManager.h"

#include <unordered_map>

namespace Engine
{
static void ReplaceRecursive(const SceneRef &scene, Entity &oldEntity, Entity& newEntity)
{
	scene->ReplaceEntity(oldEntity, newEntity);
    oldEntity.GetComponent<TransformComponent>().Scale = newEntity.GetComponent<TransformComponent>().Scale;

	const auto &newParent = newEntity.GetComponent<ParentComponent>();
	const auto &oldParent = oldEntity.GetComponent<ParentComponent>();

	for (size_t i = newParent.Children.size(); i < oldParent.Children.size(); i++)
    {
        auto child = scene->GetEntityByUUID(oldParent.Children[i]);
        scene->DestroyEntityRecursive(child);
    }

	for (size_t i = 0; i < newParent.Children.size(); i++)
	{
		auto newChild = scene->GetEntityByUUID(newParent.Children[i]);
		
		if (i >= oldParent.Children.size())
		{
            auto duplicateChildEntity = scene->DuplicateEntityRecursive(newChild, {});
			oldEntity.AddChild(duplicateChildEntity);

            auto &dcp = duplicateChildEntity.AddOrReplaceComponent<PrefabInstanceComponent>();
			dcp.PrefabID = newEntity.GetComponent<PrefabInstanceComponent>().PrefabID;
			auto &ncp = newChild.AddOrReplaceComponent<PrefabInstanceComponent>();
			ncp.PrefabID = newEntity.GetComponent<PrefabInstanceComponent>().PrefabID;
        }
        else
        {
			auto oldChild = scene->GetEntityByUUID(oldParent.Children[i]);
			ReplaceRecursive(scene, oldChild, newChild);
        }
	}
}

void Prefab::Apply(const SceneRef &scene, Entity &newEntity)
{
	// Replace all prefabs with the new entity
    auto prefabView = scene->GetRegistry().view<PrefabInstanceComponent, TagComponent>();
	for (auto e : prefabView)
	{
		const auto &prefab = prefabView.get<PrefabInstanceComponent>(e);
		const auto &tag = prefabView.get<TagComponent>(e);

		// Make sure that the entity being replaced is a prefab root
		if (prefab.PrefabID == Handle && tag.IsPrefabRoot)
		{
			auto ent = Entity{e, scene.get()};
			ReplaceRecursive(scene, ent, newEntity);
		}
	}

	{
        // Serialize the prefab with the new entity
        auto path = Project::GetAssetDirectory() / AssetManager::GetRegistry()[Handle].FilePath;
        PrefabSerializer serializer(scene);
        serializer.Serialize(path, newEntity);
    }

	if (scene->GetSceneType() == SceneType::Scene3D) 
	{
        // save scene
        SceneSerializer serializer(scene);
        serializer.Serialize(scene->GetSceneFilePath());
    }
}

void Prefab::Revert(const SceneRef &scene, Entity &entity) const
{
    auto path = Project::GetAssetDirectory() / AssetManager::GetRegistry()[Handle].FilePath;

    PrefabSerializer serializer(scene);
    auto prefabEntity = serializer.Deserialize(path);

	ReplaceRecursive(scene, entity, prefabEntity);

	scene->DestroyEntityRecursive(prefabEntity);
}

void Prefab::OpenInIsolation(Entity &prefabEntity)
{
	auto scene = SceneManager::Get().GetActiveScene();
    scene->SetSceneType(SceneType::Prefab3D);
	scene->SetCurrentPrefabScene(prefabEntity.GetComponent<IDComponent>().ID);
	LOG_CORE_CRITICAL("Prefab ID: {0}", prefabEntity.GetComponent<IDComponent>().ID);

	for (auto e : scene->GetRegistry().view<TagComponent, VisibilityComponent, ParentComponent, IDComponent>())
	{
		auto entity = Entity{e, scene.get()};
        entity.GetComponent<VisibilityComponent>().IsVisible = false;

        if (entity.GetComponent<IDComponent>().ID == prefabEntity.GetComponent<IDComponent>().ID 
			|| entity.GetComponent<ParentComponent>().Parent == prefabEntity.GetComponent<IDComponent>().ID)
        {
			entity.GetComponent<VisibilityComponent>().IsVisible = true;
        }
	}

	// add previous transform component
	auto &transform = prefabEntity.GetComponent<TransformComponent>();
	auto &prevTransform = prefabEntity.AddComponent<PrevTransformComponent>();
	prevTransform.Translation = transform.Translation;
	prevTransform.Rotation = transform.Rotation;
	prevTransform.Scale = transform.Scale;

	transform .Translation = glm::vec3(0.0f);
	transform.Rotation = glm::quat(1, 0, 0, 0);

	// reset editor camera
	scene->GetEditorCamera()->Reset();
}
}
