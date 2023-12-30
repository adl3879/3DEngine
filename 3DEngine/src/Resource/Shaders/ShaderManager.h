#pragma once

#include <map>
#include <string>
#include <memory>

#include "Shader.h"

namespace Engine
{
class ShaderManager
{
  public:
    static Shader *GetShader(const std::string &path);

  private:
    static std::map<std::string, std::unique_ptr<Shader>> m_Shaders;
};
} // namespace Engine