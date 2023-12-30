#include "MeshFileManager.h"

#include "Log.h"
#include "AssetManager.h"

#include <fstream>

namespace Engine
{
bool MeshFileManager::WriteMeshFile(const std::filesystem::path &path, const MeshRef &mesh)
{
    if (std::filesystem::exists(path)) LOG_CORE_WARN("Overwriting existing mesh file: {0}", path.string());

    std::ofstream file(path, std::ios::binary);
    if (!file.is_open())
    {
        LOG_CORE_ERROR("Failed to open mesh file: {0}", path.string());
        return false;
    }

    // Version
    uint16_t version = 0x0001U;
    file.write(reinterpret_cast<char *>(&version), sizeof(uint16_t));

    // Name
    std::string name = mesh->Name;
    auto nameLength = static_cast<uint32_t>(name.length());
    file.write(reinterpret_cast<char *>(&nameLength), sizeof(uint32_t));
    file.write(name.c_str(), nameLength);

    // Vertex data
    auto vertexCount = static_cast<uint32_t>(mesh->Vertices.size());
    file.write(reinterpret_cast<char *>(&vertexCount), sizeof(uint32_t));
    file.write(reinterpret_cast<char *>(mesh->Vertices.data()), sizeof(Vertex) * vertexCount);

    // Index data
    auto indexCount = static_cast<uint32_t>(mesh->Indices.size());
    file.write(reinterpret_cast<char *>(&indexCount), sizeof(uint32_t));
    file.write(reinterpret_cast<char *>(mesh->Indices.data()), sizeof(uint32_t) * indexCount);

    // Material handle
    uint64_t materialHandle = mesh->DefaultMaterialHandle;
    file.write(reinterpret_cast<char *>(&materialHandle), sizeof(uint64_t));

    file.close();

    return true;
}

MeshRef MeshFileManager::ReadMeshFile(const std::filesystem::path &path)
{
    std::ifstream file(path, std::ios::binary);
    if (!file.is_open())
    {
        LOG_CORE_ERROR("Failed to open mesh file: {0}", path.string());
        return nullptr;
    }

    // Version
    uint16_t version;
    file.read(reinterpret_cast<char *>(&version), sizeof(uint16_t));

    // Name
    uint32_t nameLength;
    file.read(reinterpret_cast<char *>(&nameLength), sizeof(uint32_t));
    std::string name(nameLength, '\0');
    file.read(name.data(), nameLength);

    // Vertex data
    uint32_t vertexCount;
    file.read(reinterpret_cast<char *>(&vertexCount), sizeof(uint32_t));
    std::vector<Vertex> vertices(vertexCount);
    file.read(reinterpret_cast<char *>(vertices.data()), sizeof(Vertex) * vertexCount);

    // Index data
    uint32_t indexCount;
    file.read(reinterpret_cast<char *>(&indexCount), sizeof(uint32_t));
    std::vector<uint32_t> indices(indexCount);
    file.read(reinterpret_cast<char *>(indices.data()), sizeof(uint32_t) * indexCount);

    // Material handle
    uint64_t materialHandle;
    file.read(reinterpret_cast<char *>(&materialHandle), sizeof(uint64_t));

    file.close();

    return std::make_shared<Mesh>(name, vertices, indices, materialHandle);
}
} // namespace Engine
