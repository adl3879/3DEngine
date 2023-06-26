#include "SceneSerializer.h"

#include <fstream>

#include "Components.h"
#include "Log.h"

namespace Engine
{
YAML::Emitter &operator<<(YAML::Emitter &out, const glm::vec3 &v)
{
    out << YAML::Flow;
    out << YAML::BeginSeq << v.x << v.y << v.z << YAML::EndSeq;
    return out;
}

static void SerializeEntity(YAML::Emitter &out, Entity entity)
{
    out << YAML::BeginMap; // Entity

    out << YAML::Key << "Entity" << YAML::Value << "123456789";

    if (entity.HasComponent<TagComponent>())
    {
        out << YAML::Key << "TagComponent";
        out << YAML::BeginMap; // TagComponent

        auto &tag = entity.GetComponent<TagComponent>().Tag;
        out << YAML::Key << "Tag" << YAML::Value << tag;

        out << YAML::EndMap; // TagComponent
    }

    if (entity.HasComponent<TransformComponent>())
    {
        out << YAML::Key << "TransformComponent";
        out << YAML::BeginMap; // TransformComponent

        auto &tc = entity.GetComponent<TransformComponent>();
        out << YAML::Key << "Translation" << YAML::Value << tc.Translation;
        out << YAML::Key << "Rotation" << YAML::Value << tc.Rotation;
        out << YAML::Key << "Scale" << YAML::Value << tc.Scale;

        out << YAML::EndMap; // TransformComponent
    }

    if (entity.HasComponent<CameraComponent>())
    {
        out << YAML::Key << "CameraComponent";
        out << YAML::BeginMap; // CameraComponent

        auto &cameraComponent = entity.GetComponent<CameraComponent>();
        auto &camera = cameraComponent.Camera;
        out << YAML::Key << "Camera" << YAML::Value;
        out << YAML::BeginMap; // Camera
        out << YAML::Key << "ProjectionType" << YAML::Value << (int)camera.GetProjectionType();
        out << YAML::Key << "PerspectiveFOV" << YAML::Value << camera.GetPerspectiveVerticalFOV();
        out << YAML::Key << "PerspectiveNear" << YAML::Value << camera.GetPerspectiveNearClip();
        out << YAML::Key << "PerspectiveFar" << YAML::Value << camera.GetPerspectiveFarClip();
        out << YAML::Key << "OrthographicSize" << YAML::Value << camera.GetOrthographicSize();
        out << YAML::Key << "OrthographicNear" << YAML::Value << camera.GetOrthographicNearClip();
        out << YAML::Key << "OrthographicFar" << YAML::Value << camera.GetOrthographicFarClip();
        out << YAML::EndMap; // Camera

        out << YAML::Key << "Primary" << YAML::Value << cameraComponent.Primary;

        out << YAML::EndMap; // CameraComponent
    }

    if (entity.HasComponent<ModelComponent>())
    {
        out << YAML::Key << "ModelComponent";
        out << YAML::BeginMap; // ModelComponent

        auto &modelComponent = entity.GetComponent<ModelComponent>();
        out << YAML::Key << "ModelPath" << YAML::Value << modelComponent.Path;

        out << YAML::EndMap; // ModelComponent
    }

    if (entity.HasComponent<LuaScriptComponent>())
    {
        out << YAML::Key << "LuaScriptComponent";
        out << YAML::BeginMap; // LuaScriptComponent

        auto &luaScriptComponent = entity.GetComponent<LuaScriptComponent>();
        out << YAML::Key << "Paths" << YAML::Flow;
        out << YAML::BeginSeq;
        for (auto &script : luaScriptComponent.Paths) out << script;
        out << YAML::EndSeq;

        out << YAML::Key << "Names" << YAML::Flow;
        out << YAML::BeginSeq;
        for (auto &script : luaScriptComponent.Names) out << script;
        out << YAML::EndSeq;

        out << YAML::EndMap; // LuaScriptComponent
    }

    out << YAML::EndMap; // Entity
}

void SceneSerializer::Serialize(const std::string &filepath)
{
    YAML::Emitter out;
    out << YAML::BeginMap;
    out << YAML::Key << "Scene" << YAML::Value << "Untitled";
    out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;
    m_Scene->m_Registry.each(
        [&](auto entityId)
        {
            Entity entity{entityId, m_Scene.get()};
            if (!entity) return;

            SerializeEntity(out, entity);
        });
    out << YAML::EndSeq;
    out << YAML::EndMap;

    std::ofstream fout(filepath);
    fout << out.c_str();
}

void SceneSerializer::SerializeRuntime(const std::string &filepath) {}

bool SceneSerializer::Deserialize(const std::string &filepath)
{
    std::ifstream stream(filepath);
    std::stringstream strStream;
    strStream << stream.rdbuf();

    YAML::Node data = YAML::Load(strStream.str());
    if (!data["Scene"]) return false;

    std::string sceneName = data["Scene"].as<std::string>();
    LOG_CORE_TRACE("Deserializing scene from file: {0}", sceneName);

    auto entities = data["Entities"];
    if (entities)
    {
        for (auto entity : entities)
        {
            uint64_t uuid = entity["Entity"].as<uint64_t>(); // TODO: use uuid

            std::string name;
            auto tagComponent = entity["TagComponent"];
            if (tagComponent) name = tagComponent["Tag"].as<std::string>();

            LOG_CORE_TRACE("Deserialized entity with ID = {0}, name = {1}", uuid, name);

            Entity deserializedEntity = m_Scene->CreateEntity(name);

            auto transformComponent = entity["TransformComponent"];
            if (transformComponent)
            {
                auto &tc = deserializedEntity.GetComponent<TransformComponent>();
                tc.Translation = transformComponent["Translation"].as<glm::vec3>();
                tc.Rotation = transformComponent["Rotation"].as<glm::vec3>();
                tc.Scale = transformComponent["Scale"].as<glm::vec3>();
            }

            auto cameraComponent = entity["CameraComponent"];
            if (cameraComponent)
            {
                auto cc = deserializedEntity.AddComponent<CameraComponent>();
                auto cameraProps = cameraComponent["Camera"];
                cc.Camera.SetProjectionType((Camera::ProjectionType)cameraProps["ProjectionType"].as<int>());

                cc.Camera.SetPerspectiveVerticalFOV(cameraProps["PerspectiveFOV"].as<float>());
                cc.Camera.SetPerspectiveNearClip(cameraProps["PerspectiveNear"].as<float>());
                cc.Camera.SetPerspectiveFarClip(cameraProps["PerspectiveFar"].as<float>());

                cc.Camera.SetOrthographicSize(cameraProps["OrthographicSize"].as<float>());
                cc.Camera.SetOrthographicNearClip(cameraProps["OrthographicNear"].as<float>());
                cc.Camera.SetOrthographicFarClip(cameraProps["OrthographicFar"].as<float>());

                cc.Primary = cameraComponent["Primary"].as<bool>();
            }

            auto modelComponent = entity["ModelComponent"];
            if (modelComponent)
            {
                auto path = modelComponent["ModelPath"].as<std::string>();
                deserializedEntity.AddComponent<ModelComponent>(path);
            }

            auto luaScriptComponent = entity["LuaScriptComponent"];
            if (luaScriptComponent)
            {
                auto &lsc = deserializedEntity.AddComponent<LuaScriptComponent>();
                for (size_t i = 0; i < luaScriptComponent["Paths"].size(); i++)
                {
                    auto path = luaScriptComponent["Paths"][i].as<std::string>();
                    auto name = luaScriptComponent["Names"][i].as<std::string>();
                    lsc.Bind(path, name);
                }
            }
        }
    }

    return true;
}

bool SceneSerializer::DeserializeRuntime(const std::string &filepath)
{
    assert(false);
    return false;
}
} // namespace Engine