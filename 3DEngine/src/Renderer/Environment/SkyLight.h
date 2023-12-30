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
    void Destroy();
    void Render(const glm::mat4 &projection, const glm::mat4 &view);

public:
    unsigned int GetIrradianceMap() { return m_IrradianceMap; }
    unsigned int GetPrefilterMap() { return m_PreFilterMap; }
    unsigned int GetBrdfLUT() { return m_BrdfLUT; }
    const std::unordered_map<std::string, ShaderRef> &GetShaders() { return m_Shaders; }

    [[nodiscard]] AssetHandle GetHandle() const { return Handle; }

    void BindMaps(int slot = 0) const;
    void UnBindMaps() const;

    [[nodiscard]] std::filesystem::path GetHdrPath() const { return m_HdrPath; }

public:
    static AssetType GetStaticType() { return AssetType::SkyLight; }
    [[nodiscard]] AssetType GetType() const override { return GetStaticType(); }

private:
    void RenderQuad();

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
