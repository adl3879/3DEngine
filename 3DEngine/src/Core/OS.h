#pragma once

#include <string>
#include <filesystem>

namespace Engine
{
class OS
{
public:
    static void OpenIn(const std::string &filePath);
    static void ExecuteCommand(const std::string &command);
};
}
