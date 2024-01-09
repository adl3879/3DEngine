#include "MeshImporter.h"

#include "SkinnedModel.h"
#include "ImportData.h"

namespace Engine
{                              
static bool LoadStaticMesh(const std::filesystem::path &path, const std::filesystem::path &currentDir)
{
	// Copy mesh file into currentDir
    auto copyMeshFileTo = std::filesystem::current_path() / currentDir / path.filename();
    if (currentDir != "")
		std::filesystem::copy(path, copyMeshFileTo, std::filesystem::copy_options::overwrite_existing);

	// create a import data 
	ImportData importData;
	importData.Type = AssetType::Mesh;
	importData.SourceFile = path;
	
	UUID uuid;
    std::string dstFileName = std::format("{0}-{1}.mesh", path.filename().string(), uuid.ToString());
	importData.DestinationFile = std::filesystem::current_path() / Project::GetImportCachePath() / dstFileName;

	auto model = std::make_shared<Model>();
	if (!model->LoadModel(path)) return false;

	// write import data to disk
	ImportDataSerializer serializer(importData);
	auto importDataPath = path.string() + ".import";
	serializer.Serialize(importDataPath);
	
	if (MeshFileManager::WriteMeshFile(importData.DestinationFile, model))
		LOG_CORE_INFO("Successfully wrote mesh to disk");
	else
		LOG_CORE_ERROR("Failed to write mesh to disk");

    return true;
}

static bool LoadSkinnedMesh(const std::filesystem::path &path, const std::filesystem::path &currentDir)
{
    return true;
}

ModelRef MeshImporter::ImportMesh(AssetHandle handle, const AssetMetadata &metadata)
{
 	auto importDataFile = metadata.FilePath.string() + ".import";
	if (!std::filesystem::exists(Project::GetAssetDirectory() / importDataFile))
	{
		LoadModel(std::filesystem::current_path() / Project::GetAssetDirectory() / metadata.FilePath, "");
	}

	ImportData importData;
	ImportDataSerializer serializer(importData);
	serializer.Deserialize(Project::GetAssetDirectory() / importDataFile);

	if (!std::filesystem::exists(importData.DestinationFile))
	{
        if (LoadModel(std::filesystem::current_path() / Project::GetAssetDirectory() / metadata.FilePath, ""))
            serializer.Deserialize(Project::GetAssetDirectory() / importDataFile);
        else return nullptr;
	}

	auto meshes = MeshFileManager::ReadMeshFile(importData.DestinationFile);
	if (meshes.empty()) return nullptr;

	auto model = std::make_shared<Model>();
	for (auto mesh : meshes)
	{
		model->AttachMesh(std::move(mesh));
	}

	return model;
}

bool MeshImporter::LoadModel(const std::filesystem::path &path, const std::filesystem::path &currentDir)
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

    ModelRef model = std::make_shared<Model>(path, currentDir);
    if (!model->HasAnimations())
        return LoadStaticMesh(path, currentDir);
    else
        return LoadSkinnedMesh(path, currentDir);

    return true;
}

bool MeshImporter::ReloadModel(const std::filesystem::path &path, const std::filesystem::path &to)
{
	LoadModel(std::filesystem::current_path() / path, to);

	return true;
}
}
