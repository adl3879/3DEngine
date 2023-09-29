#pragma once

#include "Project.h"

namespace Engine
{
class ProjectSerializer
{
  public:
    ProjectSerializer(ProjectPtr project);

    bool Serialize(const std::filesystem::path &filepath);
    bool Deserialize(const std::filesystem::path &filepath);

  private:
    ProjectPtr m_Project;
};

} // namespace Engine