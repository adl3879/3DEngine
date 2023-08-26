#include "EditorAssetManager.h"

#include "AssetImporter.h"
#include "Log.h"
#include "Project.h"

#include <yaml-cpp/yaml.h>
#include <fstream>

namespace Engine
{
YAML::Emitter &operator<<(YAML::Emitter &out, const std::string_view &v)
{
    out << std::string(v.data(), v.size());
    return out;
}

bool EditorAssetManager::IsAssetHandleValid(AssetHandle handle) const
{
    return handle != 0 && m_AssetRegistry.find(handle) != m_AssetRegistry.end();
}

bool EditorAssetManager::IsAssetLoaded(AssetHandle handle) const
{
    return m_LoadedAssets.find(handle) != m_LoadedAssets.end();
}

AssetHandle EditorAssetManager::ImportAsset(const std::filesystem::path &path)
{
    if (path.filename().empty()) return 0;
    AssetHandle handle; // generate new handle
    AssetMetadata metadata;
    metadata.FilePath = path;
    metadata.Type = Utils::GetAssetTypeFromExtension(path);

    AssetRef asset = AssetImporter::ImportAsset(handle, metadata);
    if (asset == nullptr) return 0;
    asset->Handle = handle;
    if (asset)
    {
        m_LoadedAssets[handle] = asset;
        m_AssetRegistry[handle] = metadata;
        SerializeAssetRegistry();
    }
    return handle;
}

const AssetMetadata &EditorAssetManager::GetMetadata(AssetHandle handle) const
{
    static AssetMetadata s_NullMetadata;
    auto it = m_AssetRegistry.find(handle);
    if (it == m_AssetRegistry.end()) return s_NullMetadata;

    return it->second;
}

void EditorAssetManager::SerializeAssetRegistry()
{
    auto path = Project::GetAssetRegistryPath();

    YAML::Emitter out;
    {
        out << YAML::BeginMap; // Root
        out << YAML::Key << "AssetRegistry" << YAML::Value;

        out << YAML::BeginSeq;
        for (const auto &[handle, metadata] : m_AssetRegistry)
        {
            out << YAML::BeginMap;
            out << YAML::Key << "Handle" << YAML::Value << handle;
            std::string filepathStr = metadata.FilePath.generic_string();
            out << YAML::Key << "FilePath" << YAML::Value << filepathStr;
            out << YAML::Key << "Type" << YAML::Value << AssetTypeToString(metadata.Type);
            out << YAML::EndMap;
        }
        out << YAML::EndSeq;
        out << YAML::EndMap; // Root
    }

    std::ofstream fout(path);
    fout << out.c_str();
}

bool EditorAssetManager::DeserializeAssetRegistry()
{
    auto path = Project::GetAssetRegistryPath();
    YAML::Node data;
    try
    {
        data = YAML::LoadFile(path.string());
    }
    catch (YAML::ParserException e)
    {
        LOG_CORE_ERROR("Failed to load project file '{0}'\n     {1}", path.string(), e.what());
        return false;
    }

    auto rootNode = data["AssetRegistry"];
    if (!rootNode) return false;

    for (const auto &node : rootNode)
    {
        AssetHandle handle = node["Handle"].as<uint64_t>();
        auto &metadata = m_AssetRegistry[handle];
        metadata.FilePath = node["FilePath"].as<std::string>();
        metadata.Type = AssetTypeFromString(node["Type"].as<std::string>());
    }

    return true;
}

AssetRef EditorAssetManager::GetAsset(AssetHandle handle)
{
    // 1. check if handle is valid
    if (!IsAssetHandleValid(handle)) return nullptr;

    // 2. check if asset needs load (and if so, load)
    AssetRef asset;
    if (IsAssetLoaded(handle))
    {
        asset = m_LoadedAssets.at(handle);
    }
    else
    {
        // load asset
        const AssetMetadata &metadata = GetMetadata(handle);
        asset = AssetImporter::ImportAsset(handle, metadata);
        m_LoadedAssets[handle] = asset;
        if (!asset)
        {
            LOG_CORE_ERROR("EditorAssetManager::GetAsset - asset import failed!");
        }
    }
    // 3. return asset
    return asset;
}

AssetRef EditorAssetManager::GetAsset(const std::filesystem::path &path)
{
    AssetHandle handle = GetAssetHandleFromPath(path);
    if (handle == 0) handle = ImportAsset(path);

    return GetAsset(handle);
}

AssetHandle EditorAssetManager::GetAssetHandleFromPath(const std::filesystem::path &path)
{
    for (auto &[handle, metadata] : m_AssetRegistry)
    {
        if (metadata.FilePath == path) return handle;
    }
    // if it does not find it load it and return the handle
    return ImportAsset(path);
}

void EditorAssetManager::UnloadAsset(AssetHandle handle)
{
    m_AssetRegistry.erase(handle);
    SerializeAssetRegistry();
}

std::string EditorAssetManager::GetAssetName(AssetHandle handle) const
{
    auto asset = m_AssetRegistry.find(handle);
    if (asset == m_AssetRegistry.end()) return "Unknown";
    return asset->second.FilePath.filename().stem().string();
}
} // namespace Engine