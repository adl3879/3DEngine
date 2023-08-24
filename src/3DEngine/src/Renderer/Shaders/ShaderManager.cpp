#include "ShaderManager.h"

namespace Engine
{
std::map<std::string, std::unique_ptr<Shader>> ShaderManager::m_Shaders =
    std::map<std::string, std::unique_ptr<Shader>>();

Shader *ShaderManager::GetShader(const std::string &path)
{
    if (m_Shaders.find(path) == m_Shaders.end())
    {
        m_Shaders[path] = std::make_unique<Shader>(path + ".vert", path + ".frag");
    }

    return m_Shaders[path].get();
}
} // namespace Engine