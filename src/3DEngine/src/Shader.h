#pragma once

#include <glad/glad.h>

#include <glm/fwd.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>

class Shader
{
  public:
    Shader() = default;
    Shader(const std::string &vertexSourcePath, const std::string &fragmentSourcePath);

    void Use() const;
    void Delete() const;

    void SetUniform4f(std::string id, float x, float y, float w, float h);
    void SetUniformMatrix4fv(std::string id, glm::mat4 matrix);
    void SetUniform3f(std::string id, glm::vec3 vector);
    void SetUniform4f(std::string id, glm::vec4 vector);

    unsigned int GetProgram() const { return m_Program; }

  private:
    std::string ParseShader(const std::string &sourcePath);

  private:
    unsigned int m_Program;
};
