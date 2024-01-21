#pragma once

#include <filesystem>

#include "Model.h"
#include "SkinnedModel.h"

namespace Engine
{
class MeshFileManager
{
  public:
    static bool WriteMeshFile(const std::filesystem::path &path, const ModelRef &model);
    static std::vector<StaticMesh> ReadMeshFile(const std::filesystem::path &path);

	static bool WriteSkinnedMeshFile(const std::filesystem::path &path, const SkinnedModelRef &model);
    static SkinnedMeshData ReadSkinnedMeshFile(const std::filesystem::path &path);
};
} // namespace Engine
