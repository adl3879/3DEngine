#include "AssetImporter.h"

#include <functional>
#include <unordered_map>

#include "Log.h"
#include "TextureImporter.h"
#include "MeshImporter.h"
#include "SceneImporter.h"
#include "MaterialImporter.h"
#include "HDRIImporter.h"

namespace Engine
{
using AssetImportFn = std::function<AssetRef(AssetHandle, const AssetMetadata &)>;

static std::unordered_map<AssetType, AssetImportFn> s_AssetImportFns = {
    {AssetType::Texture2D, TextureImporter::ImportTexture2D},
    {AssetType::TextureHDRI, HDRIImporter::ImportHDRI},
    {AssetType::Mesh, MeshImporter::ImportMesh},
    {AssetType::Scene, SceneImporter::ImportScene},
    {AssetType::Material, MaterialImporter::ImportMaterial},
};

AssetRef AssetImporter::ImportAsset(AssetHandle handle, const AssetMetadata &metadata)
{
    if (s_AssetImportFns.find(metadata.Type) == s_AssetImportFns.end())
    {
        LOG_CORE_ERROR("No importer available for asset type: {}", (uint16_t)metadata.Type);
        return nullptr;
    }

    return s_AssetImportFns.at(metadata.Type)(handle, metadata);
}
} // namespace Engine