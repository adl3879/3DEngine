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
    m_Prefab->GetScene()->GetRegistry().each(
        [&](auto entityId)
        {
            Entity entity{entityId, m_Prefab->GetScene().get()};
            if (!entity) return;

            SceneSerializer::SerializeEntity(out, entity);
        });
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

    auto entities = data["Entities"];
    if (entities)
    {
        for (auto entity : entities)
        {
            SceneRef scene = m_Prefab->GetScene();

            auto uuid = entity["Entity"].as<uint64_t>();
            std::string name;
            auto tagComponent = entity["TagComponent"];
            if (tagComponent) name = tagComponent["Tag"].as<std::string>();

            LOG_CORE_TRACE("Deserialized entity with ID = {0}, name = {1}", uuid, name);
            Entity deserializedEntity = scene->CreateEntityWithUUID(uuid, name);
            SceneSerializer::DeserializeEntity(entity, deserializedEntity);
        }
    }
    return true;
}
} // namespace Engine
