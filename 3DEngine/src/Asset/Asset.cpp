#include "Asset.h"

namespace Engine
{
std::string_view AssetTypeToString(AssetType type)
{
    switch (type)
    {
        case AssetType::None: return "AssetType::None";
        case AssetType::Scene: return "AssetType::Scene";
        case AssetType::Texture2D: return "AssetType::Texture2D";
        case AssetType::Mesh: return "AssetType::Mesh";
        case AssetType::Material: return "AssetType::Material";
        case AssetType::SkyLight: return "AssetType::SkyLight";
        case AssetType::Prefab: return "AssetType::Prefab";
        case AssetType::Shader: return "AssetType::Shader";
        case AssetType::LuaScript: return "AssetType::LuaScript";
        case AssetType::NetScript: return "AssetType::NetScript";
        default: break;
    }

    return "AssetType::<Invalid>";
}

AssetType AssetTypeFromString(std::string_view assetType)
{
    if (assetType == "AssetType::None") return AssetType::None;
    if (assetType == "AssetType::Scene") return AssetType::Scene;
    if (assetType == "AssetType::Texture2D") return AssetType::Texture2D;
    if (assetType == "AssetType::Mesh") return AssetType::Mesh;
    if (assetType == "AssetType::Material") return AssetType::Material;
    if (assetType == "AssetType::SkyLight") return AssetType::SkyLight;
    if (assetType == "AssetType::Prefab") return AssetType::Prefab;
    if (assetType == "AssetType::Shader") return AssetType::Shader;
    if (assetType == "AssetType::LuaScript") return AssetType::LuaScript;
    if (assetType == "AssetType::NetScript") return AssetType::NetScript;

    return AssetType::None;
}
} // namespace Engine
