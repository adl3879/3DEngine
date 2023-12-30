#include "CustomShader.h"

#include <fstream>
#include <sstream>
#include <regex>

#include "Log.h"

namespace Engine
{
CustomShader::CustomShader(const std::filesystem::path &path)
{
    ShaderData shaderData = ParseShader(path);
    auto vs = GenerateVertexShader(shaderData);
    auto fs = GenerateFragmentShader(shaderData);

    m_Shader = std::make_shared<Shader>();
    m_Shader->Init(vs, fs);
}

ShaderData CustomShader::ParseShader(const std::filesystem::path &path)
{
    // Read the shader code from a file (you can load it as a string)
    std::ifstream shaderFile(path);
    std::string shaderCode((std::istreambuf_iterator<char>(shaderFile)), std::istreambuf_iterator<char>());
    shaderFile.close();

    ShaderData shaderData;

    // Define regular expressions to match shader parts and uniform declarations
    std::regex vertexRegex(R"(void vertex\(\) \{([\s\S]*?)\})");
    std::regex fragmentRegex(R"(void fragment\(\) \{([\s\S]*?)\})");
    std::regex uniformRegex("uniform .*?;");

    std::smatch match;

    // Extract the vertex shader
    if (std::regex_search(shaderCode, match, vertexRegex))
    {
        shaderData.VertexSource = match[1];
    }

    // Extract the fragment shader
    if (std::regex_search(shaderCode, match, fragmentRegex))
    {
        shaderData.FragmentSource = match[1];
    }

    // Extract uniform declarations
    auto uniformsBegin = std::sregex_iterator(shaderCode.begin(), shaderCode.end(), uniformRegex);
    auto uniformsEnd = std::sregex_iterator();
    for (std::sregex_iterator i = uniformsBegin; i != uniformsEnd; ++i)
    {
        shaderData.UniformDeclaration.push_back(i->str());
    }

    return shaderData;
}

std::string CustomShader::GenerateVertexShader(const ShaderData &shaderData)
{
    std::ifstream shaderFile("Resources/shaders/PBR.vert");
    if (!shaderFile)
    {
        LOG_CORE_INFO("Failed to open shader file.");
        return ""; // Return an empty string if the file cannot be opened.
    }

    std::string shaderCode((std::istreambuf_iterator<char>(shaderFile)), std::istreambuf_iterator<char>());

    std::map<std::string, std::string> params{
        {"POSITION", "aPos"}, {"NORMAL", "aNormal"}, {"TANGENT", "aTangent"}, {"UV", "aUV"}};

    // replace params key in shaderData.VertexSource with its value
    std::string vertexSource = shaderData.VertexSource;
    for (auto &param : params)
    {
        std::regex paramRegex(param.first);
        vertexSource = std::regex_replace(vertexSource, paramRegex, param.second);
    }

    // Replace the //custom_shader comment with the custom vertex shader source
    std::string customShaderPlaceholder = "//custom_shader";
    size_t customShaderPos = shaderCode.find(customShaderPlaceholder);
    if (customShaderPos != std::string::npos)
    {
        shaderCode.replace(customShaderPos, customShaderPlaceholder.length(), vertexSource);
    }
    else
    {
        LOG_CORE_INFO("//custom_shader not found in the shader code.");
        return ""; // Return an empty string if the placeholder is not found.
    }

    return shaderCode;
}

std::string CustomShader::GenerateFragmentShader(const ShaderData &shaderData)
{
    std::ifstream shaderFile("Resources/shaders/PBR.frag");
    if (!shaderFile)
    {
        LOG_CORE_INFO("Failed to open shader file.");
        return ""; // Return an empty string if the file cannot be opened.
    }

    std::string shaderCode((std::istreambuf_iterator<char>(shaderFile)), std::istreambuf_iterator<char>());

    std::map<std::string, std::string> params{
        {"ALBEDO", "albedo"}, {"NORMAL", "normal"}, {"METALLIC", "metallic"}, {"ROUGHNESS", "roughness"}, {"AO", "ao"}
    };

    // replace params key in shaderData.VertexSource with its value
    std::string fragmentSource = shaderData.FragmentSource;
    for (auto &param : params)
    {
        std::regex paramRegex(param.first);
        fragmentSource = std::regex_replace(fragmentSource, paramRegex, param.second);
    }

    // Replace the //custom_shader comment with the custom vertex shader source
    std::string customShaderPlaceholder = "//custom_shader";
    size_t customShaderPos = shaderCode.find(customShaderPlaceholder);
    if (customShaderPos != std::string::npos)
    {
        shaderCode.replace(customShaderPos, customShaderPlaceholder.length(), fragmentSource);
    }
    else
    {
        LOG_CORE_INFO("//custom_shader not found in the shader code.");
        return ""; // Return an empty string if the placeholder is not found.
    }

    return shaderCode;
}
} // namespace Engine
