#pragma once

#include "Asset.h"
#include "AssetMetadata.h"

#include "Material.h"
#include "MaterialSerializer.h"
#include "Project.h"

namespace Engine
{
class MaterialImporter
{
  public:
    static MaterialRef ImportMaterial(AssetHandle handle, const AssetMetadata &metadata)
    {
        MaterialRef material = std::make_shared<Material>();
		material->Handle = handle;
        MaterialSerializer serializer(material);
        serializer.Deserialize(Project::GetAssetDirectory() / metadata.FilePath);

        return material;
    }
};
} // namespace Engine
