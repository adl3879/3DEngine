#include "MaterialSerializer.h"

#include <fstream>

#include "Log.h"

#include <yaml-cpp/yaml.h>

namespace Engine
{
static void ParseVec3(YAML::Node node, glm::vec3 &value)
{
    if (node.IsSequence() && node.size() == 3)
    {
        value.x = node[0].as<float>();
        value.y = node[1].as<float>();
        value.z = node[2].as<float>();
    }
}

MaterialSerializer::MaterialSerializer(MaterialRef material) : m_Material(material) {}

bool MaterialSerializer::Serialize(const std::filesystem::path &filepath)
{
    const auto &textures = m_Material->GetTextureHandles();
    const auto &parameters = m_Material->GetMaterialData();

    YAML::Emitter out;
    {
        out << YAML::BeginMap; // Root
        out << YAML::Key << "Material" << YAML::Value;
        {
            out << YAML::BeginMap; // Material
            out << YAML::Key << "Name" << YAML::Value << std::string(m_Material->Name);
            out << YAML::Key << "AlbedoColor" << YAML::Value << YAML::Flow << YAML::BeginSeq << parameters.Albedo.x
                << parameters.Albedo.y << parameters.Albedo.z << YAML::EndSeq;
            out << YAML::Key << "Metallic" << YAML::Value << parameters.Metallic;
            out << YAML::Key << "Roughness" << YAML::Value << parameters.Roughness;
            out << YAML::Key << "AO" << YAML::Value << parameters.AO;
            // out << YAML::Key << "UseNormalMap" << YAML::Value << parameters.UseNormalMap;
            out << YAML::Key << "AlbedoMap" << YAML::Value << textures[ParameterType::ALBEDO];
            out << YAML::Key << "MetallicMap" << YAML::Value << textures[ParameterType::METALLIC];
            out << YAML::Key << "RoughnessMap" << YAML::Value << textures[ParameterType::ROUGHNESS];
            out << YAML::Key << "AOMap" << YAML::Value << textures[ParameterType::AO];
            out << YAML::Key << "NormalMap" << YAML::Value << textures[ParameterType::NORMAL];
            out << YAML::EndMap; // Material
        }
        out << YAML::EndMap; // Root
    }

    std::ofstream fout(filepath);
    fout << out.c_str();
    fout.close();

    return true;
}

bool MaterialSerializer::Deserialize(const std::filesystem::path &filepath)
{
    YAML::Node data;
    try
    {
        data = YAML::LoadFile(filepath.string());
    }
    catch (YAML::ParserException e)
    {
        LOG_CORE_ERROR("Failed to load material file '{0}'\n     {1}", filepath.string(), e.what());
        return false;
    }

    auto materialNode = data["Material"];
    if (!materialNode) return false;

    m_Material->Name = materialNode["Name"].as<std::string>();

    auto albedoHandle = materialNode["AlbedoMap"].as<uint64_t>();
    auto aoHandle = materialNode["AOMap"].as<uint64_t>();
    auto metallicHandle = materialNode["MetallicMap"].as<uint64_t>();
    auto normalHandle = materialNode["NormalMap"].as<uint64_t>();
    auto roughnessHandle = materialNode["RoughnessMap"].as<uint64_t>();

    glm::vec3 albedoColor, normalValue;
    auto acNode = materialNode["AlbedoColor"];
    auto normalNode = materialNode["Normal"];
    ParseVec3(acNode, albedoColor);
    ParseVec3(normalNode, normalValue);

    auto aoValue = materialNode["AO"].as<float>();
    auto metallicValue = materialNode["Metallic"].as<float>();
    auto roughnessValue = materialNode["Roughness"].as<float>();

    m_Material->SetTexture(ParameterType::ALBEDO, albedoHandle);
    m_Material->SetTexture(ParameterType::AO, aoHandle);
    m_Material->SetTexture(ParameterType::METALLIC, metallicHandle);
    m_Material->SetTexture(ParameterType::NORMAL, normalHandle);
    m_Material->SetTexture(ParameterType::ROUGHNESS, roughnessHandle);

    m_Material->SetMaterialParam(ParameterType::ALBEDO, albedoColor);
    m_Material->SetMaterialParam(ParameterType::AO, aoValue);
    m_Material->SetMaterialParam(ParameterType::METALLIC, metallicValue);
    m_Material->SetMaterialParam(ParameterType::NORMAL, normalValue);
    m_Material->SetMaterialParam(ParameterType::ROUGHNESS, roughnessValue);

    return true;
}
} // namespace Engine
