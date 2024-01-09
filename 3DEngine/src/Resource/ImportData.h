#pragma once

#include <string>
#include <filesystem>
#include <memory>

#include "UUID.h"
#include "Asset.h"

namespace Engine
{
struct ImportData
{
	UUID ID;
	std::filesystem::path SourceFile;
	std::filesystem::path DestinationFile;
	AssetType Type;
};

class ImportDataSerializer
{
  public:
	ImportDataSerializer(ImportData &importData) : m_ImportData(importData) {}

	bool Serialize(const std::filesystem::path &path);
	bool Deserialize(const std::filesystem::path &path);
	
  private:
	  ImportData &m_ImportData;
};
} // namespace Engine
