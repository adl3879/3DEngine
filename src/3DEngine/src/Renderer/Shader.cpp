#include "Shader.h"

#include <glad/glad.h>

#include <fstream>
#include <iostream>
#include <sstream>

#include "Log.h"
#include "PlatformUtils.h"

namespace Engine
{
Shader::Shader(const std::string &vertexSourcePath, const std::string &fragmentSourcePath)
{
    auto vPath = Utils::Path::GetAbsolute(vertexSourcePath);
    auto fPath = Utils::Path::GetAbsolute(fragmentSourcePath);

    auto vertexShader = ParseShader(vPath);
    auto fragmentShader = ParseShader(fPath);
    const GLchar *vs = vertexShader.c_str();
    const GLchar *fs = fragmentShader.c_str();

    // compile shaders
    GLint vertex, fragment;
    GLint success;
    char infoLog[512];

    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vs, nullptr);
    glCompileShader(vertex);
    // check for compile errors
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertex, 512, NULL, infoLog);
        LOG_CORE_ERROR("SHADER::VERTEX::COMPILATION_FAILED: {0}", infoLog);
    }

    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fs, nullptr);
    glCompileShader(fragment);
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertex, 512, NULL, infoLog);
        LOG_CORE_ERROR("SHADER::FRAGMENT::COMPILATION_FAILED: {0}", infoLog);
    }

    m_Program = glCreateProgram();
    glAttachShader(m_Program, vertex);
    glAttachShader(m_Program, fragment);
    glLinkProgram(m_Program);
    glGetProgramiv(m_Program, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(m_Program, 512, NULL, infoLog);
        LOG_CORE_ERROR("SHADER::PROGRAM::LINKING_FAILED: {0}", infoLog);
    }

    // can be deleted because they are already linked to program
    glDeleteShader(vertex);
    glDeleteShader(fragment);

    Use();
}

std::string Shader::ParseShader(const std::string &sourcePath)
{
    std::string shader;
    std::ifstream shaderFile(sourcePath.c_str(), std::ios::binary);
    if (shaderFile.is_open())
    {
        std::stringstream shaderStream;
        shaderStream << shaderFile.rdbuf();
        shader = shaderStream.str();
    }
    else
    {
        LOG_CORE_ERROR("Unable to open shader file: {0}", sourcePath);
    }
    shaderFile.close();
    return shader;
}

void Shader::Use() const { glUseProgram(m_Program); }

void Shader::Delete() const
{
    if (m_Program != 0) glDeleteProgram(m_Program);
}

void Shader::SetUniform4f(std::string id, float x, float y, float w, float h)
{
    GLint location = glGetUniformLocation(m_Program, id.c_str());
    glUniform4f(location, x, y, w, h);
}

void Shader::SetUniform4f(std::string id, glm::vec4 vector)
{
    GLint location = glGetUniformLocation(m_Program, id.c_str());
    glUniform4f(location, vector.x, vector.y, vector.z, vector.w);
}

void Shader::SetUniform1f(std::string id, float value)
{
    GLint location = glGetUniformLocation(m_Program, id.c_str());
    glUniform1f(location, value);
}

void Shader::SetUniform1i(std::string id, int value)
{
    GLint location = glGetUniformLocation(m_Program, id.c_str());
    glUniform1i(location, value);
}

void Shader::SetUniformMatrix4fv(std::string id, glm::mat4 matrix)
{
    GLint location = glGetUniformLocation(m_Program, id.c_str());
    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
}

void Shader::SetUniformMatrix3fv(std::string id, glm::mat3 matrix)
{
    GLint location = glGetUniformLocation(m_Program, id.c_str());
    glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
}

void Shader::SetUniform3f(std::string id, glm::vec3 vector)
{
    GLint location = glGetUniformLocation(m_Program, id.c_str());
    glUniform3f(location, vector.x, vector.y, vector.z);
}
} // namespace Engine