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
    virtual std::string GetAssetName(AssetHandle handle) const = 0;

    virtual bool IsAssetHandleValid(AssetHandle handle) const = 0;
    virtual bool IsAssetLoaded(AssetHandle handle) const = 0;
};


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
    std::string sceneExtension = ".scene";
    std::string materialExtension = ".material";
    std::string skyLightExtension = ".hdr";

    if (IsInVector(textureExtensions, extension))
        return AssetType::Texture2D;
    else if (IsInVector(meshExtensions, extension))
        return AssetType::Mesh;
    else if (extension == sceneExtension)
        return AssetType::Scene;
    else if (extension == materialExtension)
        return AssetType::Material;
    else if (extension == skyLightExtension)
        return AssetType::TextureHDRI;
    else
        return AssetType::None;
}
} // namespace Utils
} // namespace Engine
