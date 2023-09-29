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
        SceneSerializer serializer(scene);
        scene->SetSceneName(metadata.FilePath.stem().string());
        scene->SetSceneFilePath((Project::GetAssetDirectory() / metadata.FilePath).string());
        serializer.Deserialize((Project::GetAssetDirectory() / metadata.FilePath).string());

        return scene;
    }
};
} // namespace Engine
