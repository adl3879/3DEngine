#include "ScriptingSystem.h"

#include <Log.h>

#include "ScriptingEngineNet.h"
#include "Components.h"

namespace Engine
{
ScriptingSystem::ScriptingSystem(Scene *scene)
{
    m_Scene = scene;
    ScriptingEngineNet::Get().SetCurrentScene(scene);
}

bool ScriptingSystem::Init()
{
    // TODO: This is just for development purposes
    std::filesystem::copy_file(R"(C:\dev\3DEngine\ScriptCore\ScriptCore\bin\Debug\net8.0\ScriptCore.dll)",
                                   R"(C:\dev\3DEngine\Sandbox\SandboxProject\.net\ScriptCore.dll)",
                                   std::filesystem::copy_options::overwrite_existing);

    auto &scriptingEngineNet = ScriptingEngineNet::Get();
    scriptingEngineNet.Init();
    LOG_CORE_INFO(".NET ScriptingSystem initialized");

    return true;
}

void ScriptingSystem::Update(float dt) {}

void ScriptingSystem::Exit() { ScriptingEngineNet::Get().Shutdown(); }

void ScriptingSystem::FixedUpdate(float dt)
{
    for (const auto e : m_Scene->GetRegistry().view<NetScriptComponent>())
    {
        auto entity = Entity(e, m_Scene);
        auto entityScript = ScriptingEngineNet::Get().GetEntityScript(entity);
        entityScript.InvokeMethod("OnFixedUpdate", (float)dt);
    }
}

void ScriptingSystem::OnRuntimeStart()
{
    // Get all entities with a NetScriptComponent
    for (const auto e : m_Scene->GetRegistry().view<NetScriptComponent>())
    {
        auto entity = Entity(e, m_Scene);
         // registers all entity component
        ScriptingEngineNet::Get().RegisterEntityScript(entity);

        auto entityScript = ScriptingEngineNet::Get().GetEntityScript(entity);
        entityScript.InvokeMethod("OnInit");
    }
}

void ScriptingSystem::OnRuntimeStop()
{
    for (const auto e : m_Scene->GetRegistry().view<NetScriptComponent>())
    {
        auto entity = Entity(e, m_Scene);
        auto entityScript = ScriptingEngineNet::Get().GetEntityScript(entity);
        entityScript.InvokeMethod("OnDestroy");
    }
}

void ScriptingSystem::OnRuntimeUpdate(float dt)
{
    for (const auto e : m_Scene->GetRegistry().view<NetScriptComponent>())
    {
        auto entity = Entity(e, m_Scene);
        auto entityScript = ScriptingEngineNet::Get().GetEntityScript(entity);
        entityScript.InvokeMethod("OnUpdate", (float)dt);
    }
}
} // namespace Engine
