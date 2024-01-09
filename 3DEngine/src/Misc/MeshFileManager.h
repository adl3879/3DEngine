#pragma once

#include <filesystem>

#include "Model.h"

namespace Engine
{
class MeshFileManager
{
  public:
    static bool WriteMeshFile(const std::filesystem::path &path, const MeshRef &mesh);
    static bool WriteMeshFile(const std::filesystem::path &path, const ModelRef &model);
    static MeshRef ReadMeshFile2(const std::filesystem::path &path);
    static std::vector<Mesh> ReadMeshFile(const std::filesystem::path &path);
};
} // namespace Engine
