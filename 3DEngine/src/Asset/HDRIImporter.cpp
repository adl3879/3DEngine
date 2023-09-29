#include "HDRIImporter.h"

#include <stb_image.h>

#include "Log.h"
#include "Project.h"

namespace Engine
{
TextureHDRIRef HDRIImporter::ImportHDRI(AssetHandle handle, const AssetMetadata &metadata)
{
    return LoadHDRI(Project::GetAssetDirectory() / metadata.FilePath);
}

TextureHDRIRef HDRIImporter::LoadHDRI(const std::filesystem::path &path) 
{
	stbi_set_flip_vertically_on_load(true);
	int width, height, nrComponents;
	float *data = stbi_loadf(path.string().c_str(), &width, &height, &nrComponents, 0);

	if (data == nullptr)
	{
		LOG_CORE_ERROR("HDRIImporter::LoadHDRI - Could not load texture from filepath: {}", path.string());
		return nullptr;
	}

	TextureSpecification spec;
	spec.Width = width;
	spec.Height = height;

	TextureHDRIRef texture = std::make_shared<TextureHDRI>(spec, data);
	stbi_image_free(data);
	return texture;
}
} // namespace Engine
