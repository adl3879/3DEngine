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
            out << YAML::Key << "Name" << YAML::Value << m_Material->Name;
            out << YAML::Key << "TexturesDirectory" << YAML::Value << m_Material->TexturesDirectory.string();
            out << YAML::Key << "IsDefault" << YAML::Value << m_Material->IsDefault;
            // params
            out << YAML::Key << "AlbedoColor" << YAML::Value << YAML::Flow << YAML::BeginSeq << parameters.Albedo.x
                << parameters.Albedo.y << parameters.Albedo.z << YAML::EndSeq;
            out << YAML::Key << "Metallic" << YAML::Value << parameters.Metallic;
            out << YAML::Key << "Roughness" << YAML::Value << parameters.Roughness;
            out << YAML::Key << "AO" << YAML::Value << parameters.AO;
            // handles
            out << YAML::Key << "AlbedoMap" << YAML::Value << textures[ParameterType::ALBEDO];
            out << YAML::Key << "MetallicMap" << YAML::Value << textures[ParameterType::METALLIC];
            out << YAML::Key << "RoughnessMap" << YAML::Value << textures[ParameterType::ROUGHNESS];
            out << YAML::Key << "AOMap" << YAML::Value << textures[ParameterType::AO];
            out << YAML::Key << "NormalMap" << YAML::Value << textures[ParameterType::NORMAL];
            // paths
            out << YAML::Key << "AlbedoMapPath" << YAML::Value << m_Material->GetTexturePath(ParameterType::ALBEDO);
            out << YAML::Key << "MetallicMapPath" << YAML::Value << m_Material->GetTexturePath(ParameterType::METALLIC);
            out << YAML::Key << "RoughnessMapPath" << YAML::Value
                << m_Material->GetTexturePath(ParameterType::ROUGHNESS);
            out << YAML::Key << "AOMapPath" << YAML::Value << m_Material->GetTexturePath(ParameterType::AO);
            out << YAML::Key << "NormalMapPath" << YAML::Value << m_Material->GetTexturePath(ParameterType::NORMAL);
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
    m_Material->TexturesDirectory = materialNode["TexturesDirectory"].as<std::string>();
    if (materialNode["IsDefault"]) m_Material->IsDefault = materialNode["IsDefault"].as<bool>();

    glm::vec3 albedoColor, normalValue;
    auto acNode = materialNode["AlbedoColor"];
    auto normalNode = materialNode["Normal"];
    ParseVec3(acNode, albedoColor);
    ParseVec3(normalNode, normalValue);

    auto aoValue = materialNode["AO"].as<float>();
    auto metallicValue = materialNode["Metallic"].as<float>();
    auto roughnessValue = materialNode["Roughness"].as<float>();

    m_Material->SetTexture(ParameterType::ALBEDO, materialNode["AlbedoMap"].as<uint64_t>());
    m_Material->SetTexture(ParameterType::AO, materialNode["AOMap"].as<uint64_t>());
    m_Material->SetTexture(ParameterType::METALLIC, materialNode["MetallicMap"].as<uint64_t>());
    m_Material->SetTexture(ParameterType::NORMAL, materialNode["NormalMap"].as<uint64_t>());
    m_Material->SetTexture(ParameterType::ROUGHNESS, materialNode["RoughnessMap"].as<uint64_t>());

    m_Material->SetTexturePath(ParameterType::ALBEDO, materialNode["AlbedoMapPath"].as<std::string>());
    m_Material->SetTexturePath(ParameterType::AO, materialNode["AOMapPath"].as<std::string>());
    m_Material->SetTexturePath(ParameterType::METALLIC, materialNode["MetallicMapPath"].as<std::string>());
    m_Material->SetTexturePath(ParameterType::NORMAL, materialNode["NormalMapPath"].as<std::string>());
    m_Material->SetTexturePath(ParameterType::ROUGHNESS, materialNode["RoughnessMapPath"].as<std::string>());

    m_Material->SetMaterialParam(ParameterType::ALBEDO, albedoColor);
    m_Material->SetMaterialParam(ParameterType::AO, aoValue);
    m_Material->SetMaterialParam(ParameterType::METALLIC, metallicValue);
    m_Material->SetMaterialParam(ParameterType::NORMAL, normalValue);
    m_Material->SetMaterialParam(ParameterType::ROUGHNESS, roughnessValue);

    return true;
}
} // namespace Engine
