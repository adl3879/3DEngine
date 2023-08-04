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

namespace Utils
{
static bool IsInVector(const std::vector<std::string> &vector, const std::string &value)
{
    return std::find(vector.begin(), vector.end(), value) != vector.end();
}

static AssetType GetAssetTypeFromExtension(const std::filesystem::path &path)
{
    std::string extension = path.extension().string();
    std::vector<std::string> textureExtensions = {".png", ".jpg", ".jpeg", ".bmp", ".tga"};
    std::vector<std::string> meshExtensions = {".obj", ".fbx", ".dae", ".gltf", ".glb"};

    if (IsInVector(textureExtensions, extension))
        return AssetType::Texture2D;
    else if (IsInVector(meshExtensions, extension))
        return AssetType::Mesh;
    else
        return AssetType::None;
}
} // namespace Utils

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
} // namespace Engine