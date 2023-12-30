#include "SceneSerializer.h"

#include <fstream>
#include <PhysicsComponents.h>

#include "Components.h"
#include "Light.h"
#include "Log.h"
#include "Entity.h"
#include "MaterialSerializer.h"

namespace YAML
{
template <> struct convert<glm::vec2>
{
    static Node encode(const glm::vec2 &rhs)
    {
        Node node;
        node.push_back(rhs.x);
        node.push_back(rhs.y);
        return node;
    }

    static bool decode(const Node &node, glm::vec2 &rhs)
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
YAML::Emitter &operator<<(YAML::Emitter &out, const glm::vec2 &v)
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

void SceneSerializer::SerializeEntity(YAML::Emitter &out, Entity entity)
{
    out << YAML::BeginMap; // Entity

    out << YAML::Key << "Entity" << YAML::Value << entity.GetComponent<IDComponent>().ID;

    if (entity.HasComponent<TagComponent>())
    {
        out << YAML::Key << "TagComponent";
        out << YAML::BeginMap; // TagComponent

        const auto &tag = entity.GetComponent<TagComponent>();
        out << YAML::Key << "Tag" << YAML::Value << tag.Tag;

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
        for (const auto &child : parent.Children) out << child;
        out << YAML::EndSeq; // Children

        out << YAML::EndMap; // ParentComponent
    }

    if (entity.HasComponent<TransformComponent>())
    {
        out << YAML::Key << "TransformComponent";
        out << YAML::BeginMap; // TransformComponent

        const auto &tc = entity.GetComponent<TransformComponent>();
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

        const auto &cameraComponent = entity.GetComponent<CameraComponent>();
        const auto &camera = cameraComponent.Camera;
        out << YAML::Key << "Camera" << YAML::Value;
        out << YAML::BeginMap; // Camera
        out << YAML::Key << "PerspectiveFOV" << YAML::Value << camera->GetPerspectiveVerticalFOV();
        out << YAML::Key << "PerspectiveNear" << YAML::Value << camera->GetPerspectiveNearClip();
        out << YAML::Key << "PerspectiveFar" << YAML::Value << camera->GetPerspectiveFarClip();
        out << YAML::EndMap; // Camera

        out << YAML::Key << "Primary" << YAML::Value << cameraComponent.Primary;

        out << YAML::EndMap; // CameraComponent
    }

    if (entity.HasComponent<MeshComponent>())
    {
        out << YAML::Key << "MeshComponent";
        out << YAML::BeginMap; // MeshComponent

        const auto &meshComponent = entity.GetComponent<MeshComponent>();
        out << YAML::Key << "Handle" << YAML::Value << meshComponent.Handle;
        out << YAML::Key << "MaterialHandle" << YAML::Value << meshComponent.MaterialHandle;

        out << YAML::EndMap; // MeshComponent
    }

    if (entity.HasComponent<DirectionalLightComponent>())
    {
        out << YAML::Key << "DirectionalLightComponent";
        out << YAML::BeginMap;

        const auto &dlc = entity.GetComponent<DirectionalLightComponent>();
        out << YAML::Key << "Enabled" << YAML::Value << dlc.Enabled;
        out << YAML::Key << "Color" << YAML::Value << dlc.Light.Color;
        out << YAML::Key << "AmbientIntensity" << YAML::Value << dlc.Light.Intensity;

        out << YAML::EndMap;
    }

    if (entity.HasComponent<PointLightComponent>())
    {
        out << YAML::Key << "PointLightComponent";
        out << YAML::BeginMap;

        const auto &plc = entity.GetComponent<PointLightComponent>();
        out << YAML::Key << "Enabled" << YAML::Value << plc.Enabled;
        out << YAML::Key << "Color" << YAML::Value << plc.Light.Color;
        out << YAML::Key << "AmbientIntensity" << YAML::Value << plc.Light.Intensity;

        out << YAML::EndMap;
    }

    if (entity.HasComponent<SpotLightComponent>())
    {
        out << YAML::Key << "SpotLightComponent";
        out << YAML::BeginMap;

        const auto &slc = entity.GetComponent<SpotLightComponent>();
        out << YAML::Key << "Enabled" << YAML::Value << slc.Enabled;
        out << YAML::Key << "Color" << YAML::Value << slc.Light.Color;
        out << YAML::Key << "AmbientIntensity" << YAML::Value << slc.Light.Intensity;
        out << YAML::Key << "Cutoff" << YAML::Value << slc.Light.Cutoff;
        out << YAML::Key << "OuterCutoff" << YAML::Value << slc.Light.OuterCutoff;

        out << YAML::EndMap;
    }

    if (entity.HasComponent<RigidBodyComponent>())
    {
        out << YAML::Key << "RigidBodyComponent";
        out << YAML::BeginMap;

        const auto &rb = entity.GetComponent<RigidBodyComponent>();
        out << YAML::Key << "Mass" << YAML::Value << rb.Mass;
        out << YAML::Key << "MotionType" << YAML::Value << Physics::MotionTypeToString(rb.MotionType);
        out << YAML::Key << "LinearDamping" << YAML::Value << rb.LinearDamping;
        out << YAML::Key << "AngularDamping" << YAML::Value << rb.AngularDamping;
        out << YAML::Key << "IsKinematic" << YAML::Value << rb.IsKinematic;
        out << YAML::Key << "UseGravity" << YAML::Value << rb.UseGravity;

        out << YAML::EndMap;
    }

    if (entity.HasComponent<BoxColliderComponent>())
    {
        out << YAML::Key << "BoxColliderComponent";
        out << YAML::BeginMap;

        const auto &bc = entity.GetComponent<BoxColliderComponent>();
        out << YAML::Key << "Size" << YAML::Value << bc.Size;
        out << YAML::Key << "IsTrigger" << YAML::Value << bc.IsTrigger;

        out << YAML::EndMap;
    }

    if (entity.HasComponent<SphereColliderComponent>())
    {
        out << YAML::Key << "SphereColliderComponent";
        out << YAML::BeginMap;

        const auto &sc = entity.GetComponent<SphereColliderComponent>();
        out << YAML::Key << "Radius" << YAML::Value << sc.Radius;
        out << YAML::Key << "IsTrigger" << YAML::Value << sc.IsTrigger;

        out << YAML::EndMap;
    }

    if (entity.HasComponent<NetScriptComponent>())
    {
        out << YAML::Key << "NetScriptComponent";
        out << YAML::BeginMap;

        const auto &nc = entity.GetComponent<NetScriptComponent>();
        out << YAML::Key << "Handle" << YAML::Value << nc.Handle;

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
        out << YAML::Key << "HDRIHandle" << YAML::Value << environment->SkyboxHDR->Handle;
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

            SceneSerializer::SerializeEntity(out, entity);
        });
    out << YAML::EndSeq;
    out << YAML::EndMap;

    // serialize materials
    for (const auto &[handle, metadata] : AssetManager::GetRegistry())
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

void SceneSerializer::SerializeRuntime(const std::string &filepath)
{
}

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
            m_Scene->GetEnvironment()->BloomEnabled = environment["BloomEnabled"].as<
                bool>();
        if (environment["Exposure"]) m_Scene->GetEnvironment()->Exposure = environment["Exposure"].as<float>();
        if (environment["BloomIntensity"])
            m_Scene->GetEnvironment()->BloomIntensity = environment["BloomIntensity"].as<
                float>();

        if (SkyTypeFromString(skyType) == SkyType::SkyboxHDR)
        {
            m_Scene->GetEnvironment()->SkyboxHDR = AssetManager::GetAsset<SkyLight>(hdriHandle);
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
            auto uuid = entity["Entity"].as<uint64_t>();

            std::string name;
            auto tagComponent = entity["TagComponent"];
            if (tagComponent) name = tagComponent["Tag"].as<std::string>();

            LOG_CORE_TRACE("Deserialized entity with ID = {0}, name = {1}", uuid, name);
            Entity deserializedEntity = m_Scene->CreateEntityWithUUID(uuid, name);

            SceneSerializer::DeserializeEntity(entity, deserializedEntity);
        }
    }
    return true;
}

void SceneSerializer::DeserializeEntity(YAML::detail::iterator_value entity, Entity deserializedEntity)
{
    if (auto parentComponent = entity["ParentComponent"])
    {
        auto &pc = deserializedEntity.GetComponent<ParentComponent>();
        pc.Parent = parentComponent["Parent"].as<uint64_t>();
        pc.HasParent = parentComponent["HasParent"].as<bool>();
        auto children = parentComponent["Children"];
        for (auto child : children)
        {
            auto id = child.as<uint64_t>();
            pc.Children.emplace_back(id);
        }
    }

    if (auto transformComponent = entity["TransformComponent"])
    {
        auto &tc = deserializedEntity.GetComponent<TransformComponent>();
        tc.Translation = transformComponent["Translation"].as<glm::vec3>();
        tc.Rotation = transformComponent["Rotation"].as<glm::vec3>();
        tc.Scale = transformComponent["Scale"].as<glm::vec3>();

        tc.LocalTranslation = transformComponent["LocalTranslation"].as<glm::vec3>();
        tc.LocalRotation = transformComponent["LocalRotation"].as<glm::vec3>();
        tc.LocalScale = transformComponent["LocalScale"].as<glm::vec3>();
    }

    if (auto cameraComponent = entity["CameraComponent"])
    {
        auto &cc = deserializedEntity.AddComponent<CameraComponent>();
        auto cameraProps = cameraComponent["Camera"];

        cc.Camera->SetPerspectiveVerticalFOV(cameraProps["PerspectiveFOV"].as<float>());
        cc.Camera->SetPerspectiveNearClip(cameraProps["PerspectiveNear"].as<float>());
        cc.Camera->SetPerspectiveFarClip(cameraProps["PerspectiveFar"].as<float>());

        cc.Primary = cameraComponent["Primary"].as<bool>();
    }

    if (auto modelComponent = entity["MeshComponent"])
    {
        auto handle = modelComponent["Handle"].as<uint64_t>();
        auto materialHandle = modelComponent["MaterialHandle"].as<uint64_t>();

        auto &mesh = deserializedEntity.AddComponent<MeshComponent>();
        mesh.Handle = handle;
        mesh.MaterialHandle = materialHandle;
    }

    if (auto directionalLightComponent = entity["DirectionalLightComponent"])
    {
        auto &tc = deserializedEntity.GetComponent<TransformComponent>();
        auto &dlc = deserializedEntity.AddComponent<DirectionalLightComponent>();
        dlc.Light.Direction = tc.Rotation;
        dlc.Light.Color = directionalLightComponent["Color"].as<glm::vec3>();
        dlc.Light.Intensity = directionalLightComponent["AmbientIntensity"].as<float>();
    }

    if (auto pointLightComponent = entity["PointLightComponent"])
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

    if (auto spotLightComponent = entity["SpotLightComponent"])
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

    if (auto rigidBodyComponent = entity["RigidBodyComponent"])
    {
        auto &rb = deserializedEntity.AddComponent<RigidBodyComponent>();
        rb.Mass = rigidBodyComponent["Mass"].as<float>();
        rb.MotionType = Physics::MotionTypeFromString(rigidBodyComponent["MotionType"].as<std::string>());
        rb.LinearDamping = rigidBodyComponent["LinearDamping"].as<float>();
        rb.AngularDamping = rigidBodyComponent["AngularDamping"].as<float>();
        rb.IsKinematic = rigidBodyComponent["IsKinematic"].as<bool>();
        rb.UseGravity = rigidBodyComponent["UseGravity"].as<bool>();
    }

    if (auto boxColliderComponent = entity["BoxColliderComponent"])
    {
        auto &bc = deserializedEntity.AddComponent<BoxColliderComponent>();
        bc.Size = boxColliderComponent["Size"].as<glm::vec3>();
        bc.IsTrigger = boxColliderComponent["IsTrigger"].as<bool>();
    }

    if (auto sphereColliderComponent = entity["SphereColliderComponent"])
    {
        auto &sc = deserializedEntity.AddComponent<SphereColliderComponent>();
        sc.Radius = sphereColliderComponent["Radius"].as<float>();
        sc.IsTrigger = sphereColliderComponent["IsTrigger"].as<bool>();
    }

    if (auto netScriptComponent = entity["NetScriptComponent"])
    {
        auto &nc = deserializedEntity.AddComponent<NetScriptComponent>();
        nc.Handle = netScriptComponent["Handle"].as<uint64_t>();
    }
}

bool SceneSerializer::DeserializeRuntime(const std::string &filepath)
{
    assert(false);
    return false;
}
} // namespace Engine
