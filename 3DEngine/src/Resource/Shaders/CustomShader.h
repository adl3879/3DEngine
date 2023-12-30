#pragma once

#include "ShaderManager.h"
#include "Asset.h"

#include <filesystem>
#include <vector>
#include <string>

namespace Engine
{
struct ShaderData
{
    std::string VertexSource;
    std::string FragmentSource;
    std::vector<std::string> UniformDeclaration;
};

class CustomShader : public Asset
{
  public:
    CustomShader() = default;
    explicit CustomShader(const std::filesystem::path &path);

    [[nodiscard]] ShaderRef GetShader() const { return m_Shader; }

    [[nodiscard]] AssetType GetType() const override { return AssetType::Shader; }

  private:
    ShaderData ParseShader(const std::filesystem::path &path);
    std::string GenerateVertexShader(const ShaderData &shaderData);
    std::string GenerateFragmentShader(const ShaderData &shaderData);

  private:
    ShaderRef m_Shader;
};

using CustomShaderRef = std::shared_ptr<CustomShader>;
} // namespace Engine