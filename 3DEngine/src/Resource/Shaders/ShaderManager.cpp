#include "ShaderManager.h"

namespace Engine
{
std::map<std::string, std::unique_ptr<Shader>> ShaderManager::m_Shaders =
    std::map<std::string, std::unique_ptr<Shader>>();

Shader *ShaderManager::GetShader(const std::string &path)
{
    if (m_Shaders.find(path) == m_Shaders.end())
    {
        auto geometryShaderPath = path + ".geom";
        m_Shaders[path] = std::filesystem::exists(geometryShaderPath)
                              ? std::make_unique<Shader>(path + ".vert", path + ".frag", geometryShaderPath)
                              : std::make_unique<Shader>(path + ".vert", path + ".frag");
    }

    return m_Shaders[path].get();
}
} // namespace Engine
