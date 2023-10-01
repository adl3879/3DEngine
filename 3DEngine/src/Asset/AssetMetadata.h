#pragma once

#include "Asset.h"

#include <filesystem>

namespace Engine
{
struct AssetMetadata
{
    AssetType Type = AssetType::None;
	// DO NOT REMOVE (/ASSETS) OR EVERYTHING WILL BREAK
    std::filesystem::path FilePath = "/Assets";
	bool Serializable = true;

    operator bool() const { return Type != AssetType::None; }
};
} // namespace Engine
