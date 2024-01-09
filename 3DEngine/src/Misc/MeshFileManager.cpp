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

    // Material path
    std::string materialPath = mesh->DefaultMaterialPath.string();
    auto pathLength = static_cast<uint32_t>(materialPath.length());
    file.write(reinterpret_cast<char *>(&pathLength), sizeof(uint32_t));
    file.write(materialPath.c_str(), pathLength);

    file.close();

    return true;
}

bool MeshFileManager::WriteMeshFile(const std::filesystem::path& path, const ModelRef& model)
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
	file.write(reinterpret_cast<char*>(&version), sizeof(uint16_t));

	// Name
    std::string name = model->GetName();
	auto nameLength = static_cast<uint32_t>(name.length());
	file.write(reinterpret_cast<char*>(&nameLength), sizeof(uint32_t));
	file.write(name.c_str(), nameLength);

	// iterate through meshes
	for (auto &mesh : model->GetMeshes())
	{
        // Name
        std::string name = mesh.Name;
        auto nameLength = static_cast<uint32_t>(name.length());
        file.write(reinterpret_cast<char *>(&nameLength), sizeof(uint32_t));
        file.write(name.c_str(), nameLength);

        // Vertex data
        auto vertexCount = static_cast<uint32_t>(mesh.Vertices.size());
        file.write(reinterpret_cast<char *>(&vertexCount), sizeof(uint32_t));
        file.write(reinterpret_cast<char *>(mesh.Vertices.data()), sizeof(Vertex) * vertexCount);

        // Index data
        auto indexCount = static_cast<uint32_t>(mesh.Indices.size());
        file.write(reinterpret_cast<char *>(&indexCount), sizeof(uint32_t));
        file.write(reinterpret_cast<char *>(mesh.Indices.data()), sizeof(uint32_t) * indexCount);

		// Material data
        
        if (const auto material = mesh.DefaultMaterial; material)
        {
            // Material name
            std::string materialName = material->Name;
            auto materialNameLength = static_cast<uint32_t>(materialName.length());
            file.write(reinterpret_cast<char *>(&materialNameLength), sizeof(uint32_t));
            file.write(materialName.c_str(), materialNameLength);

            // Albedo
            std::string albedo = material->GetTexturePath(ParameterType::ALBEDO);
            auto albedoLength = static_cast<uint32_t>(albedo.length());
            file.write(reinterpret_cast<char *>(&albedoLength), sizeof(uint32_t));
            file.write(albedo.c_str(), albedoLength);

            // Normal
            std::string normal = material->GetTexturePath(ParameterType::NORMAL);
            auto normalLength = static_cast<uint32_t>(normal.length());
            file.write(reinterpret_cast<char *>(&normalLength), sizeof(uint32_t));
            file.write(normal.c_str(), normalLength);

            // Metallic
            std::string metallic = material->GetTexturePath(ParameterType::METALLIC);
            auto metallicLength = static_cast<uint32_t>(metallic.length());
            file.write(reinterpret_cast<char *>(&metallicLength), sizeof(uint32_t));
            file.write(metallic.c_str(), metallicLength);

            // Roughness
            std::string roughness = material->GetTexturePath(ParameterType::ROUGHNESS);
            auto roughnessLength = static_cast<uint32_t>(roughness.length());
            file.write(reinterpret_cast<char *>(&roughnessLength), sizeof(uint32_t));
            file.write(roughness.c_str(), roughnessLength);

            // AO
            std::string ao = material->GetTexturePath(ParameterType::AO);
            auto aoLength = static_cast<uint32_t>(ao.length());
            file.write(reinterpret_cast<char *>(&aoLength), sizeof(uint32_t));
            file.write(ao.c_str(), aoLength);
        }
		else
		{
			// Material name
			std::string materialName = "";
			auto materialNameLength = static_cast<uint32_t>(materialName.length());
			file.write(reinterpret_cast<char *>(&materialNameLength), sizeof(uint32_t));
			file.write(materialName.c_str(), materialNameLength);

			// Albedo
			std::string albedo = "";
			auto albedoLength = static_cast<uint32_t>(albedo.length());
			file.write(reinterpret_cast<char *>(&albedoLength), sizeof(uint32_t));
			file.write(albedo.c_str(), albedoLength);

			// Normal
			std::string normal = "";
			auto normalLength = static_cast<uint32_t>(normal.length());
			file.write(reinterpret_cast<char *>(&normalLength), sizeof(uint32_t));
			file.write(normal.c_str(), normalLength);

			// Metallic
			std::string metallic = "";
			auto metallicLength = static_cast<uint32_t>(metallic.length());
			file.write(reinterpret_cast<char *>(&metallicLength), sizeof(uint32_t));
			file.write(metallic.c_str(), metallicLength);

			// Roughness
			std::string roughness = "";
			auto roughnessLength = static_cast<uint32_t>(roughness.length());
			file.write(reinterpret_cast<char *>(&roughnessLength), sizeof(uint32_t));
			file.write(roughness.c_str(), roughnessLength);

			// AO
			std::string ao = "";
			auto aoLength = static_cast<uint32_t>(ao.length());
			file.write(reinterpret_cast<char *>(&aoLength), sizeof(uint32_t));
			file.write(ao.c_str(), aoLength);
		}
	}
	
	file.close();

	return true;
}

std::vector<Mesh> MeshFileManager::ReadMeshFile(const std::filesystem::path& path)
{
	std::ifstream file(path, std::ios::binary);
	if (!file.is_open())
	{
		LOG_CORE_ERROR("Failed to open mesh file: {0}", path.string());
        return {};
	}

	// Version
	uint16_t version;
	file.read(reinterpret_cast<char*>(&version), sizeof(uint16_t));

	// Name
	uint32_t nameLength;
	file.read(reinterpret_cast<char*>(&nameLength), sizeof(uint32_t));
	std::string name(nameLength, '\0');
	file.read(name.data(), nameLength);

	// iterate through meshes
	std::vector<Mesh> meshes;
	while (!file.eof())
	{
		// Name
		uint32_t nameLength;
		file.read(reinterpret_cast<char*>(&nameLength), sizeof(uint32_t));
		std::string name(nameLength, '\0');
		file.read(name.data(), nameLength);

		// Vertex data
		uint32_t vertexCount;
		file.read(reinterpret_cast<char*>(&vertexCount), sizeof(uint32_t));
		std::vector<Vertex> vertices(vertexCount);
		file.read(reinterpret_cast<char*>(vertices.data()), sizeof(Vertex) * vertexCount);

		// Index data
		uint32_t indexCount;
		file.read(reinterpret_cast<char*>(&indexCount), sizeof(uint32_t));
		std::vector<uint32_t> indices(indexCount);
		file.read(reinterpret_cast<char*>(indices.data()), sizeof(uint32_t) * indexCount);

		// Material data
		// Material name
		uint32_t materialNameLength;
		file.read(reinterpret_cast<char*>(&materialNameLength), sizeof(uint32_t));
		std::string materialName(materialNameLength, '\0');
		file.read(materialName.data(), materialNameLength);

		// Albedo
		uint32_t albedoLength;
		file.read(reinterpret_cast<char*>(&albedoLength), sizeof(uint32_t));
		std::string albedo(albedoLength, '\0');
		file.read(albedo.data(), albedoLength);

		// Normal
		uint32_t normalLength;
		file.read(reinterpret_cast<char*>(&normalLength), sizeof(uint32_t));
		std::string normal(normalLength, '\0');
		file.read(normal.data(), normalLength);

		// Metallic
		uint32_t metallicLength;
		file.read(reinterpret_cast<char*>(&metallicLength), sizeof(uint32_t));
		std::string metallic(metallicLength, '\0');
		file.read(metallic.data(), metallicLength);

		// Roughness
		uint32_t roughnessLength;
		file.read(reinterpret_cast<char*>(&roughnessLength), sizeof(uint32_t));
		std::string roughness(roughnessLength, '\0');
		file.read(roughness.data(), roughnessLength);

		// AO
		uint32_t aoLength;
		file.read(reinterpret_cast<char*>(&aoLength), sizeof(uint32_t));
		std::string ao(aoLength, '\0');
		file.read(ao.data(), aoLength);

		// Create material
		auto material = std::make_shared<Material>();
		material->Init(materialName, albedo, normal, metallic, roughness, ao);

		// Create mesh
		Mesh mesh(name, vertices, indices, material);
		mesh.IndexCount = indexCount;
		mesh.VertexCount = vertexCount;

		meshes.push_back(mesh);
	}
	file.close();

	return meshes;
}

MeshRef MeshFileManager::ReadMeshFile2(const std::filesystem::path &path)
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

    // Material path
    uint32_t pathLength;
    file.read(reinterpret_cast<char *>(&pathLength), sizeof(uint32_t));
    std::string materialPath(pathLength, '\0');
    file.read(materialPath.data(), pathLength);

    file.close();

    return std::make_shared<Mesh>(name, vertices, indices, materialPath);
}
} // namespace Engine
