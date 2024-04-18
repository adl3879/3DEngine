#pragma once

#include <string>
#include <memory>
#include <unordered_map>
#include <glm/glm.hpp>
#include <filesystem>

#include "Shader.h"
#include "VertexArray.h"
#include "Camera.h"
#include "Asset.h"

namespace Engine
{
class SkyLight : public Asset
{
public:
    SkyLight(const std::filesystem::path &hdrPath);
    virtual ~SkyLight() = default;

    void Init(const std::size_t resolution);
    void Render(const glm::mat4 &projection, const glm::mat4 &view);

public:
    AssetHandle GetHandle() const { return Handle; }

    void Bind(Shader *shader) const;

    std::filesystem::path GetHdrPath() const { return m_HdrPath; }

public:
    static AssetType GetStaticType() { return AssetType::SkyLight; }
    AssetType GetType() const override { return GetStaticType(); }

private:
    void RenderQuad();

	void CreateHDRCubemap();
    void CreateConvulatedCubemap();
    void CreateSpecularCubemaps();

    void CreateBRDLUT();

private:
    unsigned int m_EnvCubemap = 0;
    unsigned int m_IrradianceMap = 0;
    unsigned int m_PreFilterMap = 0;
    unsigned int m_BrdfLUT = 0;

    VertexArray m_CubeVAO{};
    VertexArray m_QuadVAO{};

    std::unordered_map<std::string, ShaderRef> m_Shaders;
    std::filesystem::path m_HdrPath;
};

using SkyLightRef = std::shared_ptr<SkyLight>;
} // namespace Engine
