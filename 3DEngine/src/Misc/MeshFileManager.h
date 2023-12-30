#pragma once

#include <filesystem>

#include "Mesh.h"

namespace Engine
{
// creates a custom mesh file format (.mesh)
// stores the mesh data in binary format
// header:
    // 1. version number
    // 2. mesh name
    // 3. number of meshes
// body:
    // 1. vertex data + count
    // 2. index data + count
    // 3. material data

class MeshFileManager
{
  public:
    static bool WriteMeshFile(const std::filesystem::path &path, const MeshRef &mesh);
    static MeshRef ReadMeshFile(const std::filesystem::path &path);
};
} // namespace Engine
