#include "SceneSerializer.h"

#include <fstream>

#include "Components.h"
#include "Light.h"
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

    out << YAML::Key << "Entity" << YAML::Value << entity.GetComponent<IDComponent>().ID;

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
        out << YAML::Key << "PerspectiveFOV" << YAML::Value << camera.GetPerspectiveVerticalFOV();
        out << YAML::Key << "PerspectiveNear" << YAML::Value << camera.GetPerspectiveNearClip();
        out << YAML::Key << "PerspectiveFar" << YAML::Value << camera.GetPerspectiveFarClip();
        out << YAML::EndMap; // Camera

        out << YAML::Key << "Primary" << YAML::Value << cameraComponent.Primary;

        out << YAML::EndMap; // CameraComponent
    }

    if (entity.HasComponent<MeshComponent>())
    {
        out << YAML::Key << "MeshComponent";
        out << YAML::BeginMap; // MeshComponent

        auto &meshComponent = entity.GetComponent<MeshComponent>();
        out << YAML::Key << "Handle" << YAML::Value << meshComponent.Handle;
        out << YAML::Key << "MaterialHandle" << YAML::Value << meshComponent.MaterialHandle;

        out << YAML::EndMap; // MeshComponent
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

    if (entity.HasComponent<SkyLightComponent>())
    {
        out << YAML::Key << "SkyLightComponent";
        out << YAML::BeginMap;
        const auto &slc = entity.GetComponent<SkyLightComponent>();
        out << YAML::Key << "TextureHandle" << YAML::Value << slc.TextureHandle;
        out << YAML::EndMap;
    }

    if (entity.HasComponent<DirectionalLightComponent>())
    {
        out << YAML::Key << "DirectionalLightComponent";
        out << YAML::BeginMap;

        auto &dlc = entity.GetComponent<DirectionalLightComponent>();
        out << YAML::Key << "Color" << YAML::Value << dlc.Light.Color;
        out << YAML::Key << "AmbientIntensity" << YAML::Value << dlc.Light.Intensity;

        out << YAML::EndMap;
    }

    if (entity.HasComponent<PointLightComponent>())
    {
        out << YAML::Key << "PointLightComponent";
        out << YAML::BeginMap;

        auto &plc = entity.GetComponent<PointLightComponent>();
        out << YAML::Key << "Color" << YAML::Value << plc.Light.Color;
        out << YAML::Key << "AmbientIntensity" << YAML::Value << plc.Light.Intensity;

        out << YAML::EndMap;
    }

    if (entity.HasComponent<SpotLightComponent>())
    {
        out << YAML::Key << "SpotLightComponent";
        out << YAML::BeginMap;

        auto &slc = entity.GetComponent<SpotLightComponent>();
        out << YAML::Key << "Color" << YAML::Value << slc.Light.Color;
        out << YAML::Key << "AmbientIntensity" << YAML::Value << slc.Light.Intensity;
        out << YAML::Key << "Cutoff" << YAML::Value << slc.Light.Cutoff;
        out << YAML::Key << "OuterCutoff" << YAML::Value << slc.Light.OuterCutoff;

        out << YAML::EndMap;
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
            uint64_t uuid = entity["Entity"].as<uint64_t>();

            std::string name;
            auto tagComponent = entity["TagComponent"];
            if (tagComponent) name = tagComponent["Tag"].as<std::string>();

            LOG_CORE_TRACE("Deserialized entity with ID = {0}, name = {1}", uuid, name);

            Entity deserializedEntity = m_Scene->CreateEntityWithUUID(uuid, name);

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
                auto &cc = deserializedEntity.AddComponent<CameraComponent>();
                auto cameraProps = cameraComponent["Camera"];

                cc.Camera.SetPerspectiveVerticalFOV(cameraProps["PerspectiveFOV"].as<float>());
                cc.Camera.SetPerspectiveNearClip(cameraProps["PerspectiveNear"].as<float>());
                cc.Camera.SetPerspectiveFarClip(cameraProps["PerspectiveFar"].as<float>());

                cc.Primary = cameraComponent["Primary"].as<bool>();
            }

            auto modelComponent = entity["MeshComponent"];
            if (modelComponent)
            {
                auto handle = modelComponent["Handle"].as<uint64_t>();
                auto materialHandle = modelComponent["MaterialHandle"].as<uint64_t>();
                auto &mesh = deserializedEntity.AddComponent<MeshComponent>();
                mesh.Handle = handle;
                mesh.MaterialHandle = materialHandle;
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

            auto skyLightComponent = entity["SkyLightComponent"];
            if (skyLightComponent)
            {
                auto &slc = deserializedEntity.AddComponent<SkyLightComponent>();
                auto handle = skyLightComponent["TextureHandle"].as<uint64_t>();
                slc.Use(handle, 2048);
            }

            auto directionalLightComponent = entity["DirectionalLightComponent"];
            if (directionalLightComponent)
            {
                auto &tc = deserializedEntity.GetComponent<TransformComponent>();
                auto &dlc = deserializedEntity.AddComponent<DirectionalLightComponent>();
                dlc.Light.Direction = tc.Rotation;
                dlc.Light.Color = directionalLightComponent["Color"].as<glm::vec3>();
                dlc.Light.Intensity = directionalLightComponent["AmbientIntensity"].as<float>();
            }

            auto pointLightComponent = entity["PointLightComponent"];
            if (pointLightComponent)
            {
                static int count = 1;
                count++;
                auto &tc = deserializedEntity.GetComponent<TransformComponent>();

                auto &plc = deserializedEntity.AddComponent<PointLightComponent>();
                plc.Index = count;
                plc.Light.Color = pointLightComponent["Color"].as<glm::vec3>();
                plc.Light.Position = tc.Translation;
                plc.Light.Intensity = pointLightComponent["AmbientIntensity"].as<float>();
                Light::SetPointLight(plc.Light, plc.Index);
            }

            auto spotLightComponent = entity["SpotLightComponent"];
            if (spotLightComponent)
            {
                static int count = 1;
                count++;
                auto &tc = deserializedEntity.GetComponent<TransformComponent>();

                auto &slc = deserializedEntity.AddComponent<SpotLightComponent>();
                slc.Index = count;
                slc.Light.Color = spotLightComponent["Color"].as<glm::vec3>();
                slc.Light.Position = tc.Translation;
                slc.Light.Direction = tc.Rotation;
                slc.Light.Intensity = spotLightComponent["AmbientIntensity"].as<float>();
                slc.Light.Cutoff = spotLightComponent["Cutoff"].as<float>();
                slc.Light.OuterCutoff = spotLightComponent["OuterCutoff"].as<float>();
                Light::SetSpotLight(slc.Light, slc.Index);
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