#include "ProjectSerializer.h"

#include <fstream>
#include <yaml-cpp/yaml.h>

#include "Log.h"
#include "InputManager.h"

namespace Engine
{
ProjectSerializer::ProjectSerializer(ProjectRef project) : m_Project(project) {}

bool ProjectSerializer::Serialize(const std::filesystem::path &filepath)
{
    const auto &config = m_Project->GetConfig();

    YAML::Emitter out;
    {
        out << YAML::BeginMap; // Root
        out << YAML::Key << "Project" << YAML::Value;
        {
            out << YAML::BeginMap; // Project
            out << YAML::Key << "Name" << YAML::Value << config.Name;
            out << YAML::Key << "StartScene" << YAML::Value << config.StartScene.string();
            out << YAML::Key << "AssetDirectory" << YAML::Value << config.AssetDirectory.string();
            out << YAML::Key << "AssetRegistryPath" << YAML::Value << config.AssetRegistryPath.string();
            out << YAML::Key << "ScriptModulePath" << YAML::Value << config.ScriptModulePath.string();
            
			out << YAML::Key << "InputToActionMap" << YAML::Value;
			out << YAML::BeginSeq;
            
			for (const auto &action : InputManager::Get().GetInputToActionMap())
			{
				out << YAML::BeginMap;
				out << YAML::Key << "Action" << YAML::Value << action.first;
				out << YAML::Key << "Input" << YAML::Value;
				out << YAML::BeginSeq;
				for (const auto &input : action.second)
				{
					if (input.index() == 0)
					{
                        const auto &key = std::get<std::pair<SpecialKey, InputKey>>(input);
						if (key.second == InputKey::None) continue;
						out << YAML::BeginMap;
						out << YAML::Key << "SpecialKey" << YAML::Value << Utils::SpecialKeyToString(key.first);
						out << YAML::Key << "KeyboardKey" << YAML::Value << Utils::InputKeyToString(key.second);
						out << YAML::EndMap;
					}
					else if (input.index() == 1)
					{
                        const auto &key = std::get<std::pair<SpecialKey, MouseButton>>(input);
						if (key.second == MouseButton::None) continue;
						out << YAML::BeginMap;
						out << YAML::Key << "SpecialKey" << YAML::Value << Utils::SpecialKeyToString(key.first);
						out << YAML::Key << "MouseButton" << YAML::Value << Utils::MouseButtonToString(key.second);
						out << YAML::EndMap;
					}
				}
				out << YAML::EndSeq;
				out << YAML::EndMap;
			}

			out << YAML::EndSeq;
            
			out << YAML::EndMap; // Project
        }
        out << YAML::EndMap; // Root
    }

    std::ofstream fout(filepath);
    fout << out.c_str();

    return true;
}

bool ProjectSerializer::Deserialize(const std::filesystem::path &filepath)
{
    auto &config = m_Project->GetConfig();

    YAML::Node data;
    try
    {
        data = YAML::LoadFile(filepath.string());
    }
    catch (YAML::ParserException e)
    {
        LOG_CORE_ERROR("Failed to load project file '{0}'\n     {1}", filepath.string(), e.what());
        return false;
    }

    auto projectNode = data["Project"];
    if (!projectNode) return false;

    config.Name = projectNode["Name"].as<std::string>();
    config.StartScene = projectNode["StartScene"].as<std::string>();
    config.AssetDirectory = projectNode["AssetDirectory"].as<std::string>();
    if (projectNode["AssetRegistryPath"]) 
		config.AssetRegistryPath = projectNode["AssetRegistryPath"].as<std::string>();
    config.ScriptModulePath = projectNode["ScriptModulePath"].as<std::string>();

	auto inputToActionMapNode = projectNode["InputToActionMap"];
	if (inputToActionMapNode)
    {
		for (const auto& action : inputToActionMapNode)
		{
            std::string actionName = action["Action"].as<std::string>();
			for (const auto& input : action["Input"])
			{
				if (input["KeyboardKey"])
				{
					std::string specialKey = input["SpecialKey"].as<std::string>();
					std::string keyboardKey = input["KeyboardKey"].as<std::string>();
                    InputManager::Get().MapInputToAction(
                        actionName, std::make_pair(Utils::SpecialKeyFromString(specialKey), Utils::InputKeyFromString(keyboardKey)));
				}
				else if (input["MouseButton"])
				{
					std::string specialKey = input["SpecialKey"].as<std::string>();
					std::string mouseButton = input["MouseButton"].as<std::string>();
					InputManager::Get().MapInputToAction(
						actionName, std::make_pair(Utils::SpecialKeyFromString(specialKey), Utils::MouseButtonFromString(mouseButton)));
				}
			}
		}
    }

    return true;
}
} // namespace Engine
