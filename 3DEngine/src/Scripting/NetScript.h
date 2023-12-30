#pragma once

#include "Asset.h"

#include <Project.h>
#include <memory>
#include <filesystem>
#include <string>

namespace Engine
{
const std::string NET_TEMPLATE_SCRIPT_FIRST = R"(using Engine.Net;

namespace )";

const std::string NET_TEMPLATE_SCRIPT_SECOND = R"(
{
    class )";

const std::string NET_TEMPLATE_SCRIPT_THIRD = R"( : Entity
    {
        public override void OnInit()
        {
            // Called once at the start of the game
        }


        public override void OnUpdate(float dt)
        {
            // Called every frame
        }

        public override void OnFixedUpdate(float dt)
        {
            // Called every fixed update
        }


        public override void OnDestroy()
        {
            // Called at the end of the game fixed update
        }
    }
}
)";

class NetScript : public Asset
{
  public:
    NetScript() = default;
    NetScript(const std::filesystem::path &path, const std::string &className);

    void GenerateBoilerplate();

    [[nodiscard]] const std::filesystem::path &GetPath() const { return m_Path; }
    [[nodiscard]] std::string GetClassName() const { return Project::GetProjectName() + "." + m_ClassName; }

    [[nodiscard]] AssetType GetType() const override { return AssetType::NetScript; }

  private:
    std::filesystem::path m_Path;
    std::string m_ClassName;
};

using NetScriptRef = std::shared_ptr<NetScript>;
} // namespace Engine