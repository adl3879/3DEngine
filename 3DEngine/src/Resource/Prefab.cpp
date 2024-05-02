#include "Prefab.h"

#include "PrefabSerializer.h"
#include "AssetManager.h"
#include "Components.h"

#include <unordered_map>

namespace Engine
{
static void ReplaceRecursive(const SceneRef &scene, Entity &oldEntity, Entity& newEntity)
{
	auto transform = oldEntity.GetComponent<TransformComponent>();
	scene->ReplaceEntity(oldEntity, newEntity);
	// translation is not copied over
	oldEntity.GetComponent<TransformComponent>().Translation = transform.Translation;

	const auto &oldParent = oldEntity.GetComponent<ParentComponent>();
	const auto &newParent = newEntity.GetComponent<ParentComponent>();

	for (size_t i = 0; i < oldParent.Children.size(); i++)
	{
		auto oldChild = scene->GetEntityByUUID(oldParent.Children[i]);
		auto newChild = scene->GetEntityByUUID(newParent.Children[i]);
		ReplaceRecursive(scene, oldChild, newChild);
	}
}

void Prefab::Apply(const SceneRef &scene, Entity &newEntity)
{
	// Serialize the prefab with the new entity
   /* auto path = Project::GetAssetDirectory() / AssetManager::GetRegistry()[Handle].FilePath;
    PrefabSerializer serializer(scene);
    serializer.Serialize(path, newEntity);*/

	// Replace all prefabs with the new entity
    auto prefabView = scene->GetRegistry().view<PrefabInstanceComponent, IDComponent, TagComponent>();
	for (auto e : prefabView)
	{
		const auto &prefab = prefabView.get<PrefabInstanceComponent>(e);
		const auto &id = prefabView.get<IDComponent>(e);
		const auto &tag = prefabView.get<TagComponent>(e);

		if (prefab.PrefabID == Handle && tag.IsPrefabRoot)
		{
			auto ent = Entity{e, scene.get()};
			ReplaceRecursive(scene, ent, newEntity);
		}
	}
}

void Prefab::Revert(const SceneRef &scene, Entity &entity) const
{
    auto path = Project::GetAssetDirectory() / AssetManager::GetRegistry()[Handle].FilePath;
    PrefabSerializer serializer(scene);
    auto prefabEntity = serializer.Deserialize(path);

	//auto entities = FlattenEntity(scene, entity);
	//auto prefabEntities = FlattenEntity(scene, prefabEntity);

	//for (auto &[id, ent] : entities)
	//{
	//	auto prefabIt = prefabEntities.find(id);
	//	if (prefabIt != prefabEntities.end())
	//	{
	//		auto &transform = ent.GetComponent<TransformComponent>();
	//		scene->ReplaceEntity(ent, prefabIt->second);
	//		// translation is not copied over
	//		entity.GetComponent<TransformComponent>().Translation = transform.Translation;
	//	}
	//}
}
}
