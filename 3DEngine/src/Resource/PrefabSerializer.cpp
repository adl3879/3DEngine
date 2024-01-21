#include "PrefabSerializer.h"

#include "SceneSerializer.h"
#include "Log.h"
#include "Components.h"
#include <yaml-cpp/yaml.h>

#include <fstream>

namespace Engine
{
void PrefabSerializer::Serialize(const std::filesystem::path &path)
{
    YAML::Emitter out;
    out << YAML::BeginMap;
    out << YAML::Key << "Prefab" << YAML::Value << "Untitled";
    out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;
    auto i = m_Prefab->GetScene();
  
	for (auto entity : m_Prefab->GetEntities())
	{
        if (!entity) return;

        SceneSerializer::SerializeEntity(out, entity);
	}

    out << YAML::EndSeq;
    out << YAML::EndMap;

    std::ofstream fout(path);
    fout << out.c_str();
}

bool PrefabSerializer::Deserialize(const std::filesystem::path &path)
{
    std::ifstream stream(path);
    std::stringstream strStream;
    strStream << stream.rdbuf();

    YAML::Node data = YAML::Load(strStream.str());
    if (!data["Prefab"]) return false;

	std::map<UUID, UUID> oldToNewUUIDMap = {};

    auto entities = data["Entities"];
    if (entities)
    {
        for (auto entity : entities)
        {
            SceneRef scene = m_Prefab->GetScene();

			// change uuid
            auto uuid = entity["Entity"].as<uint64_t>();
			// not finding in old means it has not been changed
            auto newUUID = oldToNewUUIDMap.find(uuid) != oldToNewUUIDMap.end() ? oldToNewUUIDMap[uuid] : UUID();
			oldToNewUUIDMap[uuid] = newUUID;

            std::string name;
            auto tagComponent = entity["TagComponent"];
            if (tagComponent) name = tagComponent["Tag"].as<std::string>();

            LOG_CORE_TRACE("Deserialized entity with ID = {0}, name = {1}", uuid, name);
            Entity deserializedEntity = scene->CreateEntityWithUUID(newUUID, name);
            SceneSerializer::DeserializeEntity(entity, deserializedEntity);

			auto &tag = deserializedEntity.GetComponent<TagComponent>();
            tag.IsPrefab = true;

			// change parent
			auto &parent = deserializedEntity.GetComponent<ParentComponent>();
            if (parent.HasParent)
            {
                auto newParentUUID = oldToNewUUIDMap.find(parent.Parent) != oldToNewUUIDMap.end()
                                         ? oldToNewUUIDMap[parent.Parent]
                                         : UUID();
                oldToNewUUIDMap[parent.Parent] = newParentUUID;
                parent.Parent = newParentUUID;
            }

			// change children
			for (auto &id : parent.Children)
			{
				auto newChildUUID =
					oldToNewUUIDMap.find(id) != oldToNewUUIDMap.end() ? oldToNewUUIDMap[id] : UUID();
				oldToNewUUIDMap[id] = newChildUUID;
				id = newChildUUID;
			}
        }
    }
    return true;
}
} // namespace Engine
