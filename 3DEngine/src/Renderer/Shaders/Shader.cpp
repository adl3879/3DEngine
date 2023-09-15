#include "Shader.h"

#include <glad/glad.h>

#include <fstream>
#include <iostream>
#include <sstream>

#include "Log.h"

namespace Engine
{
Shader::Shader(const std::string &vertexSourcePath, const std::string &fragmentSourcePath)
{
    auto vertexShader = ParseShader(vertexSourcePath);
    auto fragmentShader = ParseShader(fragmentSourcePath);
    const GLchar *vs = vertexShader.c_str();
    const GLchar *fs = fragmentShader.c_str();

    // compile shaders
    int vertex, fragment;
    int success;
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

    Bind();
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

void Shader::Bind() const { glUseProgram(m_Program); }

void Shader::Unbind() const
{
    if (m_Program != 0) glUseProgram(0);
}

void Shader::Delete() const
{
    if (m_Program != 0) glDeleteProgram(m_Program);
}

void Shader::SetUniform4f(std::string id, glm::vec4 vector)
{
    int addr = FindUniformLocation(id);
    if (addr != -1) glUniform4f(addr, vector.x, vector.y, vector.z, vector.w);
}

void Shader::SetUniform1f(std::string id, float value)
{
    GLint location = glGetUniformLocation(m_Program, id.c_str());
    glUniform1f(location, value);
}

void Shader::SetUniform1f(uint32_t location, float value) { glUniform1f(location, value); }

void Shader::SetUniform1i(std::string id, int value)
{
    GLint location = glGetUniformLocation(m_Program, id.c_str());
    glUniform1i(location, value);
}

void Shader::SetUniform1i(uint32_t location, int value) { glUniform1i(location, value); }

int Shader::FindUniformLocation(const std::string &uniform)
{
    if (m_UniformLocations.find(uniform) == m_UniformLocations.end())
    {
        int addr = glGetUniformLocation(m_Program, uniform.c_str());
        if (addr == -1)
            return addr;
        else
            m_UniformLocations[uniform] = addr;

        return addr;
    }
    return m_UniformLocations[uniform];
}

void Shader::SetUniformMatrix4fv(std::string id, glm::mat4 matrix)
{
    int addr = FindUniformLocation(id);
    if (addr != -1) glUniformMatrix4fv(addr, 1, GL_FALSE, glm::value_ptr(matrix));
}

void Shader::SetUniformMatrix4fv(uint32_t location, glm::mat4 matrix)
{
    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
}

void Shader::SetUniformMatrix3fv(std::string id, glm::mat3 matrix)
{
    int addr = FindUniformLocation(id);
    if (addr != -1) glUniformMatrix3fv(addr, 1, GL_FALSE, glm::value_ptr(matrix));
}

void Shader::SetUniform3f(std::string id, glm::vec3 vector)
{
    int addr = FindUniformLocation(id);
    if (addr != -1) glUniform3f(addr, vector.x, vector.y, vector.z);
}
} // namespace Engine
