#include "NetScript.h"

#include <fstream>
#include "Project.h"
#include "Log.h"

namespace Engine
{

NetScript::NetScript(const std::filesystem::path &path, const std::string &className)
    : m_Path(path), m_ClassName(className)
{
}

void NetScript::GenerateBoilerplate()
{
    // check if file content is empty
    if (std::filesystem::file_size(m_Path) == 0)
    {
        LOG_CORE_INFO("Generating boilerplate for script: {}", m_Path.string());

        std::ofstream file(m_Path);

        if (file.is_open())
        {
            file << NET_TEMPLATE_SCRIPT_FIRST << Project::GetProjectName() << NET_TEMPLATE_SCRIPT_SECOND << m_ClassName
                 << NET_TEMPLATE_SCRIPT_THIRD;

            file.close();
        }
        else
        {
            LOG_CORE_ERROR("Failed to generate boilerplate for script: {}", m_Path.string());
        }
    }
}
} // namespace Engine