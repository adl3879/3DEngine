#pragma once

#include "SkinnedModel.h"
#include "StaticMesh.h"

#include"Asset.h"

#include <map>

namespace Engine
{
struct Mesh : public Asset
{
	std::vector<StaticMesh> StaticMeshes;

	// animation
	Engine::SkinnedMeshData SkinnedMeshData;

	bool HasAnimations() const { return !SkinnedMeshData.SkinnedMeshes.empty(); }

	[[nodiscard]] AssetType GetType() const override { return AssetType::Mesh; }
};
using MeshRef = std::shared_ptr<Mesh>;
}
