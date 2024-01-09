#include "ImportData.h"

#include <yaml-cpp/yaml.h>
#include <fstream>

namespace Engine
{
bool ImportDataSerializer::Serialize(const std::filesystem::path& path)
{
	YAML::Emitter out;
	out << YAML::BeginMap;
	out << YAML::Key << "ID" << YAML::Value << m_ImportData.ID;
	out << YAML::Key << "SourceFile" << YAML::Value << m_ImportData.SourceFile.string();
	out << YAML::Key << "DestinationFile" << YAML::Value << m_ImportData.DestinationFile.string();
	out << YAML::Key << "Type" << YAML::Value << std::string(AssetTypeToString(m_ImportData.Type));
	out << YAML::EndMap;

	std::ofstream fout(path);
	fout << out.c_str();

	return true;
}

bool ImportDataSerializer::Deserialize(const std::filesystem::path& path)
{
	std::ifstream stream(path);
	std::stringstream strStream;
	strStream << stream.rdbuf();

	YAML::Node data = YAML::Load(strStream.str());
	if (!data["ID"]) return false;

	m_ImportData.ID = data["ID"].as<uint64_t>();
	m_ImportData.SourceFile = data["SourceFile"].as<std::string>();
	m_ImportData.DestinationFile = data["DestinationFile"].as<std::string>();
	m_ImportData.Type = AssetTypeFromString(data["Type"].as<std::string>());

	return true;
}
}
