#include "PrefabSerializer.h"

#include "SceneSerializer.h"
#include "Log.h"
#include "Components.h"
#include <yaml-cpp/yaml.h>

#include <fstream>

namespace Engine
{
std::vector<Entity> PrefabSerializer::FlattenEntity(Entity entity)
{
	std::vector<Entity> entities;
	entities.push_back(entity);

	auto &parent = entity.GetComponent<ParentComponent>();
	for (auto& child : parent.Children)
	{
		auto childEntity = Entity{m_Scene->GetEntityByUUID(child), m_Scene.get()};
		auto children = FlattenEntity(childEntity);

		entities.insert(entities.end(), children.begin(), children.end());
	}

	return entities;
}

void PrefabSerializer::Serialize(const std::filesystem::path &path, Entity e)
{
    YAML::Emitter out;
    out << YAML::BeginMap;
    out << YAML::Key << "Prefab" << YAML::Value << path.stem().string();
    out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;

	e.GetComponent<TagComponent>().IsPrefabRoot = true;
    e.GetComponent<TagComponent>().IsFirstChild = true;

	for (const auto& entity : FlattenEntity(e)) SceneSerializer::SerializeEntity(out, entity);
    
    out << YAML::EndSeq;

    std::ofstream f_out(path);
    f_out << out.c_str();
}

Entity PrefabSerializer::Deserialize(const std::filesystem::path &path)
{
    std::ifstream stream(path);
    std::stringstream strStream;
    strStream << stream.rdbuf();

    YAML::Node data = YAML::Load(strStream.str());
    if (!data["Prefab"]) return {};

	std::map<UUID, UUID> oldToNewUUIDMap = {};
    Entity rootPrefab;

    auto entities = data["Entities"];
    if (entities)
    {
        for (auto entity : entities)
        {
			// change uuid
            auto uuid = entity["Entity"].as<uint64_t>();
			// not finding in old means it has not been changed
            auto newUUID = oldToNewUUIDMap.find(uuid) != oldToNewUUIDMap.end() ? oldToNewUUIDMap[uuid] : UUID();
			oldToNewUUIDMap[uuid] = newUUID;

            std::string name;
            auto tagComponent = entity["TagComponent"];
            if (tagComponent) name = tagComponent["Tag"].as<std::string>();

            LOG_CORE_TRACE("Deserialized entity with ID = {0}, name = {1}", uuid, name);
            Entity deserializedEntity = m_Scene->CreateEntityWithUUID(newUUID, name);
            SceneSerializer::DeserializeEntity(entity, deserializedEntity);

			// change parent
			auto &parent = deserializedEntity.GetComponent<ParentComponent>();
            if (parent.HasParent)
            {
                auto newParentUUID = oldToNewUUIDMap.find(parent.Parent) != oldToNewUUIDMap.end() ? oldToNewUUIDMap[parent.Parent] : UUID();
                oldToNewUUIDMap[parent.Parent] = newParentUUID;
                parent.Parent = newParentUUID;
            }

			// change children
			for (auto &id : parent.Children)
			{
				auto newChildUUID = oldToNewUUIDMap.find(id) != oldToNewUUIDMap.end() ? oldToNewUUIDMap[id] : UUID();
				oldToNewUUIDMap[id] = newChildUUID;
				id = newChildUUID;
			}

			// add prefab instance component
			auto &prefab = deserializedEntity.AddOrReplaceComponent<PrefabInstanceComponent>();
			prefab.PrefabID = AssetManager::GetAssetHandleFromPath(std::filesystem::relative(path, Project::GetAssetDirectory()));
			auto &prefabTag = deserializedEntity.GetComponent<TagComponent>();

			// add root prefab to scene root
            if (tagComponent["IsPrefabRoot"] && tagComponent["IsPrefabRoot"].as<bool>())
            {
				m_Scene->AddToRootEntity(deserializedEntity);
				rootPrefab = deserializedEntity;
				prefabTag.IsPrefabRoot = true;
            }
        }
    }

    return rootPrefab;
}
} // namespace Engine
