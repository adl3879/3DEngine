#pragma once

#include "Entity.h"
#include "NetModules/NetAPIModule.h"

namespace Coral
{
class HostInstance;
class Type;
} // namespace Coral

#include <Coral/Assembly.hpp>

namespace Engine
{
class ScriptingEngineNet
{
  public:
    static ScriptingEngineNet &Get();

    void Init();
    void Shutdown();

    void LoadAssembly();
    void LoadProjectAssembly();
    void ReloadAssembly();

    void RegisterEntityScript(Entity &entity);
    Coral::ManagedObject GetEntityScript(Entity &entity);
    bool HasEntityScriptInstance(Entity &entity);

    void GenerateSolution();
    void CreateEntityScript(const std::string &entityName);

    void SetCurrentScene(Scene *scene) { m_Scene = scene; }
    Scene *GetCurrentScene() { return m_Scene; }

  private:
    Scene *m_Scene = nullptr;

    const std::string m_Scope = "ScriptCore.Net";
    const std::string m_EngineAssemblyName = "ScriptCore.dll";
    const std::string m_NetDirectory = ".net";
    const std::string m_ContextName = "EngineContext";

    Coral::HostInstance *m_HostInstance;
    Coral::AssemblyLoadContext m_LoadContext;

    std::unordered_map<std::string, Coral::AssemblyLoadContext *> m_LoadedAssemblies;
    std::vector<std::shared_ptr<NetAPIModule>> m_Modules;

    Coral::ManagedAssembly m_EngineAssembly; // Engine DLL
    Coral::ManagedAssembly m_GameAssembly;   // Game DLL

    // This is a map that contains all the instances of entity scripts.
    std::unordered_map<std::string, Coral::Type *> m_GameEntityTypes;
    std::unordered_map<uint32_t, Coral::ManagedObject> m_EntityToManagedObjects;

    ScriptingEngineNet();
    ~ScriptingEngineNet();
};
} // namespace Engine
