#pragma once

#include "Project.h"

namespace Engine
{
class ProjectSerializer
{
  public:
    ProjectSerializer(ProjectRef project);

    bool Serialize(const std::filesystem::path &filepath);
    bool Deserialize(const std::filesystem::path &filepath);

  private:
    ProjectRef m_Project;
};

} // namespace Engine