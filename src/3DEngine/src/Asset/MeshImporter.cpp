#include "MeshImporter.h"

#include "Model.h"
#include "Project.h"

namespace Engine
{
ModelRef MeshImporter::ImportMesh(AssetHandle handle, const AssetMetadata &metadata)
{
    return std::make_shared<Model>(Project::GetAssetDirectory() / metadata.FilePath);
}
} // namespace Engine