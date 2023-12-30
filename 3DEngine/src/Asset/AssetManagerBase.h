#pragma once

#include "Asset.h"

#include <map>
#include <filesystem>

namespace Engine
{
using AssetMap = std::map<AssetHandle, AssetRef>;

class AssetManagerBase
{
public:
    virtual AssetRef GetAsset(AssetHandle handle) = 0;
    virtual AssetRef GetAsset(const std::filesystem::path &path) = 0;
    virtual AssetHandle GetAssetHandleFromPath(const std::filesystem::path &path) = 0;
    virtual void UnloadAsset(AssetHandle handle) = 0;
    [[nodiscard]] virtual std::string GetAssetName(AssetHandle handle) const = 0;

    [[nodiscard]] virtual bool IsAssetHandleValid(AssetHandle handle) const = 0;
    [[nodiscard]] virtual bool IsAssetLoaded(AssetHandle handle) const = 0;
};

namespace Utils
{
static bool IsInVector(const std::vector<std::string> &vector, const std::string &value)
{
    return std::find(vector.begin(), vector.end(), value) != vector.end();
}

static AssetType GetAssetTypeFromExtension(const std::filesystem::path &path)
{
    const std::string extension = path.extension().string();
    const std::vector<std::string> textureExtensions = {".png", ".jpg", ".jpeg", ".bmp", ".tga"};
    const std::vector<std::string> meshExtensions = {".obj", ".fbx", ".dae", ".gltf", ".glb", ".mesh"};
    const std::string sceneExtension = ".scene";
    const std::string materialExtension = ".material";
    const std::string skyLightExtension = ".hdr";
    const std::string prefabExtension = ".prefab";
    const std::string shaderExtension = ".shader";
    const std::string luaScriptExtension = ".lua";
    const std::string netScriptExtension = ".cs";

    if (IsInVector(textureExtensions, extension)) return AssetType::Texture2D;
    if (IsInVector(meshExtensions, extension)) return AssetType::Mesh;
    if (extension == sceneExtension) return AssetType::Scene;
    if (extension == materialExtension) return AssetType::Material;
    if (extension == skyLightExtension) return AssetType::SkyLight;
    if (extension == prefabExtension) return AssetType::Prefab;
    if (extension == shaderExtension) return AssetType::Shader;
    if (extension == luaScriptExtension) return AssetType::LuaScript;
    if (extension == netScriptExtension) return AssetType::NetScript;

    return AssetType::None;
}
} // namespace Utils
} // namespace Engine
