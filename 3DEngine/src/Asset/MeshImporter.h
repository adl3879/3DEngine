#pragma once

#include "Asset.h"
#include "AssetMetadata.h"
#include "AssetManager.h"

#include "Model.h"
#include "Model.h"
#include "Project.h"
#include "Log.h"
#include "Misc/MeshFileManager.h"
#include "MaterialSerializer.h"
#include "Entity.h"
#include "Components.h"
#include "Prefab.h"
#include "PrefabSerializer.h"

#include <filesystem>

namespace Engine
{
class MeshImporter
{
public:
    static MeshRef ImportMesh(AssetHandle handle, const AssetMetadata &metadata)
    {
        return MeshFileManager::ReadMeshFile(Project::GetAssetDirectory() / metadata.FilePath);
    }

    static bool LoadModel(const std::filesystem::path &path, const std::filesystem::path &currentDir)
    {
        if (!std::filesystem::exists(path))
        {
            LOG_CORE_ERROR("File {} doesn't exist", path.string());
            return false;
        }

        if (Utils::GetAssetTypeFromExtension(path) != AssetType::Mesh)
        {
            LOG_CORE_ERROR("Not a valid mesh file");
            return false;
        }

        // load as a model
        ModelRef model = std::make_shared<Model>(path, currentDir);

        // create folder to house model (TODO: make temp till saved)
        auto modelDir = std::filesystem::current_path() / currentDir / path.stem();
        if (!std::filesystem::exists(modelDir)) std::filesystem::create_directory(modelDir);

        auto scene = std::make_unique<Scene>();
        Entity rootEntity = scene->CreateEntity(path.stem().string());

        // write mesh material to disk
        int i = 0;
        for (const Mesh &mesh : model->GetMeshes())
        {
            // write model to a .mesh binary file
            if (MeshFileManager::WriteMeshFile(modelDir / (mesh.Name + ".mesh"), std::make_shared<Mesh>(mesh)))
                LOG_CORE_INFO("Successfully wrote mesh to disk");
            else
                LOG_CORE_ERROR("Failed to write mesh to disk");

            MaterialRef material = AssetManager::GetAsset<Material>(mesh.DefaultMaterialHandle);
            material->TexturesDirectory =
                std::filesystem::relative(currentDir / path.stem(), Project::GetAssetDirectory());

            MaterialSerializer serializer(material);
            std::string matName =
                material->Name.empty() ? (path.stem().string() + "_mat" + std::to_string(i)) : material->Name;
            material->Name = matName;
            serializer.Serialize(modelDir / (matName + ".material"));

            // create prefab
            // create an entity and add child entities for each mesh (loaded from .mesh asset handles)
            Entity entity = scene->CreateEntity(mesh.Name);
            auto &m = entity.AddComponent<MeshComponent>();
            m.Handle = AssetManager::GetAssetHandleFromPath(
                std::filesystem::relative(currentDir, Project::GetAssetDirectory()) / path.stem() /
                (mesh.Name + ".mesh"));
            rootEntity.AddChild(entity);

            i++;
        }

        auto prefab = std::make_shared<Prefab>();
        prefab->CreateFromEntity(rootEntity);
        PrefabSerializer serializer(prefab);
        serializer.Serialize(currentDir / path.stem() / (path.stem().string() + ".prefab"));

        return true;
    }
};
} // namespace Engine
