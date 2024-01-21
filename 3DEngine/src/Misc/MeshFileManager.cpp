#include "MeshFileManager.h"

#include "Log.h"
#include "AssetManager.h"

#include <fstream>

namespace Engine
{
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

std::vector<StaticMesh> MeshFileManager::ReadMeshFile(const std::filesystem::path& path)
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
	std::vector<StaticMesh> meshes;
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
		StaticMesh mesh(name, vertices, indices, material);
		mesh.IndexCount = indexCount;
		mesh.VertexCount = vertexCount;

		meshes.push_back(mesh);
	}
	file.close();

	return meshes;
}

bool MeshFileManager::WriteSkinnedMeshFile(const std::filesystem::path &path, const SkinnedModelRef &model)
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
    std::string name = model->GetName();
    auto nameLength = static_cast<uint32_t>(name.length());
    file.write(reinterpret_cast<char *>(&nameLength), sizeof(uint32_t));
    file.write(name.c_str(), nameLength);

	// Number of animations
    auto numAnims = static_cast<uint32_t>(model->GetNumAnimations());
	file.write(reinterpret_cast<char*>(&numAnims), sizeof(uint32_t));

	// bone info map
    auto boneInfoMap = model->GetBoneInfoMap();
    auto boneInfoMapSize = static_cast<uint32_t>(boneInfoMap.size());
    file.write(reinterpret_cast<char *>(&boneInfoMapSize), sizeof(uint32_t));

    for (const auto &[name, info] : boneInfoMap)
    {
        auto nameLength = static_cast<uint32_t>(name.length());
        file.write(reinterpret_cast<char *>(&nameLength), sizeof(uint32_t));
        file.write(name.c_str(), nameLength);

        auto id = static_cast<uint32_t>(info.ID);
        file.write(reinterpret_cast<char *>(&id), sizeof(uint32_t));

        auto offset = info.Offset;
        file.write(reinterpret_cast<char *>(&offset), sizeof(glm::mat4));
    }

    // bone count
    auto boneCount = model->GetBoneCount();
    file.write(reinterpret_cast<char *>(&boneCount), sizeof(uint32_t));

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
        file.write(reinterpret_cast<char *>(mesh.Vertices.data()), sizeof(SkinnedVertex) * vertexCount);

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

SkinnedMeshData MeshFileManager::ReadSkinnedMeshFile(const std::filesystem::path &path)
{
    std::ifstream file(path, std::ios::binary);
    if (!file.is_open())
    {
        LOG_CORE_ERROR("Failed to open mesh file: {0}", path.string());
        return {};
    }

    // Version
    uint16_t version;
    file.read(reinterpret_cast<char *>(&version), sizeof(uint16_t));

    // Name
    uint32_t nameLength;
    file.read(reinterpret_cast<char *>(&nameLength), sizeof(uint32_t));
    std::string name(nameLength, '\0');
    file.read(name.data(), nameLength);

	// Number of animations
	uint32_t numAnims;
	file.read(reinterpret_cast<char*>(&numAnims), sizeof(uint32_t));

	// bone info map
	uint32_t boneInfoMapSize;
	file.read(reinterpret_cast<char *>(&boneInfoMapSize), sizeof(uint32_t));

	std::map<std::string, BoneInfo> boneInfoMap;
	for (uint32_t i = 0; i < boneInfoMapSize; i++)
	{
		uint32_t nameLength;
		file.read(reinterpret_cast<char *>(&nameLength), sizeof(uint32_t));
		std::string name(nameLength, '\0');
		file.read(name.data(), nameLength);

		uint32_t id;
		file.read(reinterpret_cast<char *>(&id), sizeof(uint32_t));

		glm::mat4 offset;
		file.read(reinterpret_cast<char *>(&offset), sizeof(glm::mat4));

		boneInfoMap[name].ID = id;
		boneInfoMap[name].Offset = offset;
	}

	// bone count
	uint32_t boneCount;
	file.read(reinterpret_cast<char *>(&boneCount), sizeof(uint32_t));

    // iterate through meshes
    std::vector<SkinnedMesh> meshes;
    while (!file.eof())
    {
        // Name
        uint32_t nameLength;
        file.read(reinterpret_cast<char *>(&nameLength), sizeof(uint32_t));
        std::string name(nameLength, '\0');
        file.read(name.data(), nameLength);

        // Vertex data
        uint32_t vertexCount;
        file.read(reinterpret_cast<char *>(&vertexCount), sizeof(uint32_t));
        std::vector<SkinnedVertex> vertices(vertexCount);
        file.read(reinterpret_cast<char *>(vertices.data()), sizeof(SkinnedVertex) * vertexCount);

        // Index data
        uint32_t indexCount;
        file.read(reinterpret_cast<char *>(&indexCount), sizeof(uint32_t));
        std::vector<uint32_t> indices(indexCount);
        file.read(reinterpret_cast<char *>(indices.data()), sizeof(uint32_t) * indexCount);

        // Material data
        // Material name
        uint32_t materialNameLength;
        file.read(reinterpret_cast<char *>(&materialNameLength), sizeof(uint32_t));
        std::string materialName(materialNameLength, '\0');
        file.read(materialName.data(), materialNameLength);

        // Albedo
        uint32_t albedoLength;
        file.read(reinterpret_cast<char *>(&albedoLength), sizeof(uint32_t));
        std::string albedo(albedoLength, '\0');
        file.read(albedo.data(), albedoLength);

        // Normal
        uint32_t normalLength;
        file.read(reinterpret_cast<char *>(&normalLength), sizeof(uint32_t));
        std::string normal(normalLength, '\0');
        file.read(normal.data(), normalLength);

        // Metallic
        uint32_t metallicLength;
        file.read(reinterpret_cast<char *>(&metallicLength), sizeof(uint32_t));
        std::string metallic(metallicLength, '\0');
        file.read(metallic.data(), metallicLength);

        // Roughness
        uint32_t roughnessLength;
        file.read(reinterpret_cast<char *>(&roughnessLength), sizeof(uint32_t));
        std::string roughness(roughnessLength, '\0');
        file.read(roughness.data(), roughnessLength);

        // AO
        uint32_t aoLength;
        file.read(reinterpret_cast<char *>(&aoLength), sizeof(uint32_t));
        std::string ao(aoLength, '\0');
        file.read(ao.data(), aoLength);

        // Create material
        auto material = std::make_shared<Material>();
        material->Init(materialName, albedo, normal, metallic, roughness, ao);

        // Create mesh
        SkinnedMesh mesh(name, vertices, indices, material);
        mesh.IndexCount = indexCount;
        mesh.VertexCount = vertexCount;

        meshes.push_back(mesh);
    }
    file.close();

    SkinnedMeshData data;
	data.SkinnedMeshes = meshes;
	data.NumAnimations = numAnims;
	data.BoneInfoMap = boneInfoMap;
	data.BoneCount = boneCount;

	return data;
}
} // namespace Engine
