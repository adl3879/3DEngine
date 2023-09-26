#include "SceneSerializer.h"

#include <fstream>

#include "Components.h"
#include "Light.h"
#include "Log.h"
#include "Entity.h"
#include "ScriptEngine.h"
#include "MaterialSerializer.h"

#include <yaml-cpp/yaml.h>

namespace YAML
{
template <> struct convert<glm::vec2>
{
	static Node encode(const glm::vec2& rhs)
	{
		Node node;
		node.push_back(rhs.x);
		node.push_back(rhs.y);
		return node;
	}

	static bool decode(const Node& node, glm::vec2& rhs)
	{
		if (!node.IsSequence() || node.size() != 2) return false;

		rhs.x = node[0].as<float>();
		rhs.y = node[1].as<float>();
		return true;
	}
};

template <> struct convert<glm::vec3>
{
    static Node encode(const glm::vec3 &rhs)
    {
        Node node;
        node.push_back(rhs.x);
        node.push_back(rhs.y);
        node.push_back(rhs.z);
        return node;
    }

    static bool decode(const Node &node, glm::vec3 &rhs)
    {
        if (!node.IsSequence() || node.size() != 3) return false;

        rhs.x = node[0].as<float>();
        rhs.y = node[1].as<float>();
        rhs.z = node[2].as<float>();
        return true;
    }
};

template <> struct convert<glm::vec4>
{
    static Node encode(const glm::vec4 &rhs)
    {
        Node node;
        node.push_back(rhs.x);
        node.push_back(rhs.y);
        node.push_back(rhs.z);
        node.push_back(rhs.w);
        return node;
    }

    static bool decode(const Node &node, glm::vec4 &rhs)
    {
        if (!node.IsSequence() || node.size() != 4) return false;

        rhs.x = node[0].as<float>();
        rhs.y = node[1].as<float>();
        rhs.z = node[2].as<float>();
        rhs.w = node[3].as<float>();
        return true;
    }
};

template <> struct convert<Engine::UUID>
{
    static Node encode(const Engine::UUID &uuid)
    {
        Node node;
        node.push_back((uint64_t)uuid);
        return node;
    }

    static bool decode(const Node &node, Engine::UUID &uuid)
    {
        uuid = node.as<uint64_t>();
        return true;
    }
};
} // namespace YAML

namespace Engine
{
#define WRITE_SCRIPT_FIELD(FieldType, Type)                                                                            \
    case ScriptFieldType::FieldType: out << scriptField.GetValue<Type>(); break

#define READ_SCRIPT_FIELD(FieldType, Type)                                                                             \
    case ScriptFieldType::FieldType:                                                                                   \
    {                                                                                                                  \
        Type data = scriptField["Data"].as<Type>();                                                                    \
        fieldInstance.SetValue(data);                                                                                  \
        break;                                                                                                         \
    }

YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec2& v)
{
	out << YAML::Flow;
	out << YAML::BeginSeq << v.x << v.y << YAML::EndSeq;
	return out;
}

YAML::Emitter &operator<<(YAML::Emitter &out, const glm::vec3 &v)
{
    out << YAML::Flow;
    out << YAML::BeginSeq << v.x << v.y << v.z << YAML::EndSeq;
    return out;
}
YAML::Emitter &operator<<(YAML::Emitter &out, const glm::vec4 &v)
{
    out << YAML::Flow;
    out << YAML::BeginSeq << v.x << v.y << v.z << v.w << YAML::EndSeq;
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

	if (entity.HasComponent<ParentComponent>())
	{
        out << YAML::Key << "ParentComponent";
        out << YAML::BeginMap; // ParentComponent

		const auto &parent = entity.GetComponent<ParentComponent>();
		out << YAML::Key << "Parent" << YAML::Value << parent.Parent;
		out << YAML::Key << "HasParent" << YAML::Value << parent.HasParent;
		out << YAML::Key << "Children" << YAML::Value;
		out << YAML::BeginSeq; // Children
		for (auto child : parent.Children) out << child;
        out << YAML::EndSeq; // Children

		out << YAML::EndMap; // ParentComponent
	}

    if (entity.HasComponent<TransformComponent>())
    {
        out << YAML::Key << "TransformComponent";
        out << YAML::BeginMap; // TransformComponent

        auto &tc = entity.GetComponent<TransformComponent>();
        out << YAML::Key << "Translation" << YAML::Value << tc.Translation;
        out << YAML::Key << "Rotation" << YAML::Value << tc.Rotation;
        out << YAML::Key << "Scale" << YAML::Value << tc.Scale;

        out << YAML::Key << "LocalTranslation" << YAML::Value << tc.LocalTranslation;
        out << YAML::Key << "LocalRotation" << YAML::Value << tc.LocalRotation;
        out << YAML::Key << "LocalScale" << YAML::Value << tc.LocalScale;

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
        out << YAML::Key << "PerspectiveFOV" << YAML::Value << camera->GetPerspectiveVerticalFOV();
        out << YAML::Key << "PerspectiveNear" << YAML::Value << camera->GetPerspectiveNearClip();
        out << YAML::Key << "PerspectiveFar" << YAML::Value << camera->GetPerspectiveFarClip();
        out << YAML::EndMap; // Camera

        out << YAML::Key << "Primary" << YAML::Value << cameraComponent.Primary;

        out << YAML::EndMap; // CameraComponent
    }

	if (entity.HasComponent<ScriptComponent>())
	{
		out << YAML::Key << "ScriptComponent";
		out << YAML::BeginMap; // ScriptComponent

		auto &scriptComponent = entity.GetComponent<ScriptComponent>();
		out << YAML::Key << "ClassName" << YAML::Value << scriptComponent.ClassName;

		// Fields
        ScriptClassRef entityClass = ScriptEngine::GetEntityClass(scriptComponent.ClassName);
        const auto &fields = entityClass->GetFields();
        if (fields.size() > 0)
        {
            out << YAML::Key << "ScriptFields" << YAML::Value;
            auto &entityFields = ScriptEngine::GetScriptFieldMap(entity);
            out << YAML::BeginSeq;
            for (const auto &[name, field] : fields)
            {
                if (entityFields.find(name) == entityFields.end()) continue;

                out << YAML::BeginMap; // ScriptField
                out << YAML::Key << "Name" << YAML::Value << name;
                out << YAML::Key << "Type" << YAML::Value << Utils::ScriptFieldTypeToString(field.Type);
                out << YAML::Key << "Data" << YAML::Value;
                ScriptFieldInstance &scriptField = entityFields.at(name);

                switch (field.Type)
                {
                    WRITE_SCRIPT_FIELD(Float,	float);
                    WRITE_SCRIPT_FIELD(Double,	double);
                    WRITE_SCRIPT_FIELD(Bool,	bool);
                    WRITE_SCRIPT_FIELD(Char,	char);
                    WRITE_SCRIPT_FIELD(Byte,	int8_t);
                    WRITE_SCRIPT_FIELD(Short,	int16_t);
                    WRITE_SCRIPT_FIELD(Int,		int32_t);
                    WRITE_SCRIPT_FIELD(Long,	int64_t);
                    WRITE_SCRIPT_FIELD(UByte,	uint8_t);
                    WRITE_SCRIPT_FIELD(UShort,	uint16_t);
                    WRITE_SCRIPT_FIELD(UInt,	uint32_t);
                    WRITE_SCRIPT_FIELD(ULong,	uint64_t);
                    WRITE_SCRIPT_FIELD(Vector2, glm::vec2);
                    WRITE_SCRIPT_FIELD(Vector3, glm::vec3);
                    WRITE_SCRIPT_FIELD(Vector4, glm::vec4);
                    WRITE_SCRIPT_FIELD(Entity,	UUID);
                }
                out << YAML::EndMap; // ScriptFields
            }
            out << YAML::EndSeq;
        }

		out << YAML::EndMap; // ScriptComponent
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

    if (entity.HasComponent<DirectionalLightComponent>())
    {
        out << YAML::Key << "DirectionalLightComponent";
        out << YAML::BeginMap;

        auto &dlc = entity.GetComponent<DirectionalLightComponent>();
        out << YAML::Key << "Enabled" << YAML::Value << dlc.Enabled;
        out << YAML::Key << "Color" << YAML::Value << dlc.Light.Color;
        out << YAML::Key << "AmbientIntensity" << YAML::Value << dlc.Light.Intensity;

        out << YAML::EndMap;
    }

    if (entity.HasComponent<PointLightComponent>())
    {
        out << YAML::Key << "PointLightComponent";
        out << YAML::BeginMap;

        auto &plc = entity.GetComponent<PointLightComponent>();
        out << YAML::Key << "Enabled" << YAML::Value << plc.Enabled;
        out << YAML::Key << "Color" << YAML::Value << plc.Light.Color;
        out << YAML::Key << "AmbientIntensity" << YAML::Value << plc.Light.Intensity;

        out << YAML::EndMap;
    }

    if (entity.HasComponent<SpotLightComponent>())
    {
        out << YAML::Key << "SpotLightComponent";
        out << YAML::BeginMap;

        auto &slc = entity.GetComponent<SpotLightComponent>();
        out << YAML::Key << "Enabled" << YAML::Value << slc.Enabled;
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
    auto environment = m_Scene->GetEnvironment();

    YAML::Emitter out;
    out << YAML::BeginMap;
    out << YAML::Key << "Scene" << YAML::Value << "Untitled";
    out << YAML::Key << "Environment" << YAML::Value << YAML::BeginMap;
    out << YAML::Key << "SkyType" << YAML::Value << SkyTypeToString(environment->CurrentSkyType);
    out << YAML::Key << "AmbientColor" << YAML::Value << environment->AmbientColor;

	// Bloom
    out << YAML::Key << "BloomEnabled" << YAML::Value << environment->BloomEnabled;
    out << YAML::Key << "Exposure" << YAML::Value << environment->Exposure;
    out << YAML::Key << "BloomIntensity" << YAML::Value << environment->BloomIntensity;

    if (environment->SkyboxHDR)
    {
        out << YAML::Key << "HDRIHandle" << YAML::Value << environment->SkyboxHDR->GetHandle();
    }
    if (environment->ProceduralSkybox)
    {
        out << YAML::Key << "SurfaceRadius" << YAML::Value << environment->ProceduralSkybox->SurfaceRadius;
        out << YAML::Key << "AtmosphereRadius" << YAML::Value << environment->ProceduralSkybox->AtmosphereRadius;
        out << YAML::Key << "RayleighScattering" << YAML::Value << environment->ProceduralSkybox->RayleighScattering;
        out << YAML::Key << "MieScattering" << YAML::Value << environment->ProceduralSkybox->MieScattering;
        out << YAML::Key << "SunIntensity" << YAML::Value << environment->ProceduralSkybox->SunIntensity;
        out << YAML::Key << "CenterPoint" << YAML::Value << environment->ProceduralSkybox->CenterPoint;
        out << YAML::Key << "SunDirection" << YAML::Value << environment->ProceduralSkybox->SunDirection;
    }
    out << YAML::EndMap;

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

	// serialize materials
	for (const auto& [handle, metadata] : AssetManager::GetRegistry())
	{
        if (AssetManager::IsAssetLoaded(handle) && metadata.Type == AssetType::Material)
        {
            auto material = AssetManager::GetAsset<Material>(handle);
            if (material)
            {
                MaterialSerializer serializer(material);
                serializer.Serialize(Project::GetAssetDirectory() / metadata.FilePath);
            }
        }
	}

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

    auto environment = data["Environment"];
    if (environment)
    {
        auto skyType = environment["SkyType"].as<std::string>();
        auto ambientColor = environment["AmbientColor"].as<glm::vec4>();
        AssetHandle hdriHandle;
		if (environment["HDRIHandle"]) hdriHandle = environment["HDRIHandle"].as<uint64_t>();

        m_Scene->GetEnvironment()->CurrentSkyType = SkyTypeFromString(skyType);
        m_Scene->GetEnvironment()->AmbientColor = ambientColor;

		if (environment["BloomEnabled"]) 
			m_Scene->GetEnvironment()->BloomEnabled = environment["BloomEnabled"].as<bool>();
		if (environment["Exposure"]) 
			m_Scene->GetEnvironment()->Exposure = environment["Exposure"].as<float>();
        if (environment["BloomIntensity"])
			m_Scene->GetEnvironment()->BloomIntensity = environment["BloomIntensity"].as<float>();

        if (SkyTypeFromString(skyType) == SkyType::SkyboxHDR)
        {
            m_Scene->GetEnvironment()->SkyboxHDR = std::make_shared<SkyLight>();
            m_Scene->GetEnvironment()->SkyboxHDR->Init(hdriHandle, 2048);
        }

        if (SkyTypeFromString(skyType) == SkyType::ProceduralSky)
        {
            auto &proceduralSkybox = m_Scene->GetEnvironment()->ProceduralSkybox;
            proceduralSkybox->SurfaceRadius = environment["SurfaceRadius"].as<float>();
            proceduralSkybox->AtmosphereRadius = environment["AtmosphereRadius"].as<float>();
            proceduralSkybox->RayleighScattering = environment["RayleighScattering"].as<glm::vec3>();
            proceduralSkybox->MieScattering = environment["MieScattering"].as<glm::vec3>();
            proceduralSkybox->SunIntensity = environment["SunIntensity"].as<float>();
            proceduralSkybox->CenterPoint = environment["CenterPoint"].as<glm::vec3>();
            proceduralSkybox->SunDirection = environment["SunDirection"].as<glm::vec3>();
        }
    }

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

			auto parentComponent = entity["ParentComponent"];
			if (parentComponent)
			{
                auto &pc = deserializedEntity.GetComponent<ParentComponent>();
                pc.Parent = parentComponent["Parent"].as<uint64_t>();
				pc.HasParent = parentComponent["HasParent"].as<bool>();
				auto children = parentComponent["Children"];
				for (auto child : children)
				{
					auto id = child.as<uint64_t>();
					pc.Children.push_back(id);
				}
			}

            auto transformComponent = entity["TransformComponent"];
            if (transformComponent)
            {
                auto &tc = deserializedEntity.GetComponent<TransformComponent>();
                tc.Translation = transformComponent["Translation"].as<glm::vec3>();
                tc.Rotation = transformComponent["Rotation"].as<glm::vec3>();
                tc.Scale = transformComponent["Scale"].as<glm::vec3>();

				tc.LocalTranslation = transformComponent["LocalTranslation"].as<glm::vec3>();
                tc.LocalRotation = transformComponent["LocalRotation"].as<glm::vec3>();
                tc.LocalScale = transformComponent["LocalScale"].as<glm::vec3>();
            }

			auto scriptComponent = entity["ScriptComponent"];
            if (scriptComponent)
            {
                auto &sc = deserializedEntity.AddComponent<ScriptComponent>();
                sc.ClassName = scriptComponent["ClassName"].as<std::string>();

                auto scriptFields = scriptComponent["ScriptFields"];
                if (scriptFields)
                {
                    ScriptClassRef entityClass = ScriptEngine::GetEntityClass(sc.ClassName);
                    if (entityClass)
                    {
                        const auto &fields = entityClass->GetFields();
                        auto &entityFields = ScriptEngine::GetScriptFieldMap(deserializedEntity);

                        for (auto scriptField : scriptFields)
                        {
                            std::string name = scriptField["Name"].as<std::string>();
                            std::string typeString = scriptField["Type"].as<std::string>();
                            ScriptFieldType type = Utils::ScriptFieldTypeFromString(typeString);

                            ScriptFieldInstance &fieldInstance = entityFields[name];

                            assert(fields.find(name) != fields.end());

                            if (fields.find(name) == fields.end()) continue;

                            fieldInstance.Field = fields.at(name);

                            switch (type)
                            {
                                READ_SCRIPT_FIELD(Float,	float);
                                READ_SCRIPT_FIELD(Double,	double);
                                READ_SCRIPT_FIELD(Bool,		bool);
                                READ_SCRIPT_FIELD(Char,		char);
                                READ_SCRIPT_FIELD(Byte,		int8_t);
                                READ_SCRIPT_FIELD(Short,	int16_t);
                                READ_SCRIPT_FIELD(Int,		int32_t);
                                READ_SCRIPT_FIELD(Long,		int64_t);
                                READ_SCRIPT_FIELD(UByte,	uint8_t);
                                READ_SCRIPT_FIELD(UShort,	uint16_t);
                                READ_SCRIPT_FIELD(UInt,		uint32_t);
                                READ_SCRIPT_FIELD(ULong,	uint64_t);
                                READ_SCRIPT_FIELD(Vector2,	glm::vec2);
                                READ_SCRIPT_FIELD(Vector3,	glm::vec3);
                                READ_SCRIPT_FIELD(Vector4,	glm::vec4);
                                READ_SCRIPT_FIELD(Entity,	UUID);
                            }
                        }
                    }
                }
            }

            auto cameraComponent = entity["CameraComponent"];
            if (cameraComponent)
            {
                auto &cc = deserializedEntity.AddComponent<CameraComponent>();
                auto cameraProps = cameraComponent["Camera"];

                cc.Camera->SetPerspectiveVerticalFOV(cameraProps["PerspectiveFOV"].as<float>());
                cc.Camera->SetPerspectiveNearClip(cameraProps["PerspectiveNear"].as<float>());
                cc.Camera->SetPerspectiveFarClip(cameraProps["PerspectiveFar"].as<float>());

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
