#pragma once

#include "Asset.h"
#include "AssetMetadata.h"

#include "Scene.h"
#include "Project.h"
#include "SceneSerializer.h"
#include "Log.h"

namespace Engine
{
class SceneImporter
{
  public:
    static SceneRef ImportScene(AssetHandle handle, const AssetMetadata &metadata)
    {
        // takes filepath and loads it into the scene
        SceneRef scene = std::make_shared<Scene>();
        scene->SetSceneFilePath(Project::GetAssetDirectory() / metadata.FilePath);
        SceneSerializer serializer(scene);
        serializer.Deserialize(Project::GetAssetDirectory() / metadata.FilePath);

        return scene;
    }
};
} // namespace Engine