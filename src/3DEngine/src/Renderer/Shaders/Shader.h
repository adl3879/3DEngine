#pragma once

#include <glm/fwd.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <memory>
#include <unordered_map>

namespace Engine
{
class Texture2D;
class Shader
{
  public:
    Shader() = default;
    Shader(const std::string &vertexSourcePath, const std::string &fragmentSourcePath);

    void Bind() const;
    void Unbind() const;

    void Delete() const;

    void SetUniformMatrix4fv(std::string id, glm::mat4 matrix);
    void SetUniformMatrix4fv(uint32_t location, glm::mat4 matrix);
    void SetUniformMatrix3fv(std::string id, glm::mat3 matrix);
    void SetUniform3f(std::string id, glm::vec3 vector);
    void SetUniform4f(std::string id, glm::vec4 vector);
    void SetUniform1f(std::string id, float value);
    void SetUniform1f(uint32_t location, float value);
    void SetUniform1i(std::string id, int value);
    void SetUniform1i(uint32_t location, int value);
    void SetUniformTex(const std::string &name, Texture2D *texture, unsigned int slot);

    int FindUniformLocation(const std::string &uniform);

    unsigned int GetProgram() const { return m_Program; }

  private:
    std::string ParseShader(const std::string &sourcePath);

  private:
    unsigned int m_Program;
    std::unordered_map<std::string, int> m_UniformLocations;
};

using ShaderPtr = std::shared_ptr<Shader>;
} // namespace Engine