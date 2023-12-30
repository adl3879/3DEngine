#include "OS.h"

#define GLFW_EXPOSE_NATIVE_WIN32
#include <Windows.h>
#include <ShlObj.h>
#include <tchar.h>

namespace Engine
{
void OS::OpenIn(const std::string &filePath)
{
    ShellExecuteA(nullptr, "open", filePath.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
}

void OS::ExecuteCommand(const std::string &command)
{
    ShellExecuteA(nullptr, "open", "cmd.exe", ("/c " + command).c_str(), nullptr, SW_SHOWDEFAULT);
}
} // namespace Engine
