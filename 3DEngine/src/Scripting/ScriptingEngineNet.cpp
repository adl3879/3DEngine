#include "ScriptingEngineNet.h"

#include <Coral/HostInstance.hpp>
#include <Coral/GC.hpp>
#include <Coral/Attribute.hpp>

#include <FileWatch.h>

#include "Log.h"
#include "Project.h"
#include "Core/OS.h"
#include "Components.h"
#include "AssetManager.h"
#include "NetScript.h"

#include "NetModules/EngineNetAPI.h"
#include "NetModules/InputNetAPI.h"
#include "NetModules/SceneNetAPI.h"
#include "Utils/CustomString.h"

void ExceptionCallback(std::string_view inMessage)
{
    const std::string message = std::string("Unhandled native exception: ") + std::string(inMessage);
    LOG_CORE_CRITICAL("{}", message);
}

namespace Engine
{
struct ScriptingEngineNetData
{
    std::unique_ptr<filewatch::FileWatch<std::string>> AppAssemblyFileWatcher;
    bool AssemblyReloadPending = false;
};
ScriptingEngineNetData *s_Data = new ScriptingEngineNetData();

static void OnAppAssemblyFileSystemEvent(const std::string &path, const filewatch::Event change_type)
{
    if (!s_Data->AssemblyReloadPending && change_type == filewatch::Event::modified)
    {
        s_Data->AssemblyReloadPending = true;

        Application::SubmitToMainThread(
            []()
            {
                s_Data->AppAssemblyFileWatcher.reset();
                ScriptingEngineNet::Get().ReloadAssembly();
            });
    }
}

ScriptingEngineNet::ScriptingEngineNet()
{
    // Initialize Coral
    const auto coralDir = R"(C:\dev\3DEngine\ScriptCore\ScriptCore\bin\Debug\net8.0)"; // TODO: fix this
    const Coral::HostSettings settings = {.CoralDirectory = coralDir, .ExceptionCallback = ExceptionCallback};
    m_HostInstance = new Coral::HostInstance();
    m_HostInstance->Initialize(settings);

    // Initialize API modules
    m_Modules = {
        std::make_shared<EngineNetAPI>(),
        std::make_shared<InputNetAPI>(),
        std::make_shared<SceneNetAPI>(),
    };

    for (const auto &m : m_Modules)
    {
        m->RegisterMethods();
    }
}

ScriptingEngineNet::~ScriptingEngineNet() { m_HostInstance->Shutdown(); }

ScriptingEngineNet &ScriptingEngineNet::Get()
{
    static ScriptingEngineNet instance;
    return instance;
}

void ScriptingEngineNet::Init()
{
    LoadAssembly();
    LoadProjectAssembly();
}

void ScriptingEngineNet::Shutdown()
{
    // We have to manually destroy every managed object we have created
    for (auto &[entity, managedObject] : m_EntityToManagedObjects)
    {
        managedObject.Destroy();
    }

    Coral::GC::Collect(1, Coral::GCCollectionMode::Forced);
    Coral::GC::WaitForPendingFinalizers();

    m_HostInstance->UnloadAssemblyLoadContext(m_LoadContext);

    m_EntityToManagedObjects.clear();
    m_GameEntityTypes.clear();
}

void ScriptingEngineNet::LoadAssembly()
{
    m_LoadContext = m_HostInstance->CreateAssemblyLoadContext(m_ContextName);

    const auto engineAssemblyPath = R"(C:\dev\3DEngine\ScriptCore\ScriptCore\bin\Debug\net8.0\ScriptCore.dll)"; // TODO: fix this
    m_EngineAssembly = m_LoadContext.LoadAssembly(engineAssemblyPath);

    // Upload internal calls for each module
    for (const auto &netModule : m_Modules)
    {
        // const std::string inClassName = m_Scope + '.' + netModule->GetModuleName();
        for (const auto &[methodName, methodPtr] : netModule->GetMethods())
        {
            auto classNameSplit = String::Split(methodName, '.');
            m_EngineAssembly.AddInternalCall(m_Scope + "." + classNameSplit[0], classNameSplit[1], methodPtr);
        }
    }

    m_EngineAssembly.UploadInternalCalls();
}

void ScriptingEngineNet::LoadProjectAssembly()
{
    const auto scriptModulePath = std::filesystem::current_path() / Project::GetProjectName() / m_NetDirectory;
    const auto assemblyPath =
        scriptModulePath / "bin/Any CPU/Debug/net8.0" / (Project::GetProjectName() + ".dll"); // TODO: fix this

    m_GameAssembly = m_LoadContext.LoadAssembly(assemblyPath.string());

    s_Data->AppAssemblyFileWatcher =
        std::make_unique<filewatch::FileWatch<std::string>>(assemblyPath.string(), OnAppAssemblyFileSystemEvent);

    for (auto &type : m_GameAssembly.GetTypes())
    {
        LOG_CORE_INFO("Detected type: {}", std::string(type->GetFullName()));
        LOG_CORE_INFO("Detected base type: {}", std::string(type->GetBaseType().GetFullName()));
		
        const auto baseTypeName = std::string(type->GetBaseType().GetFullName());
        if (baseTypeName == "ScriptCore.Net.Entity")
        {
            // We have found an entity script.
            m_GameEntityTypes[std::string(type->GetFullName())] = type;

			for (auto &fields : type->GetFields())
            {
                LOG_CORE_INFO("Name: {0}, Field Name: {1}, Type: {2}", std::string(type->GetFullName()),
                              std::string(fields.GetName()), std::string(fields.GetType().GetFullName()));
            }
        }
    }
}

void ScriptingEngineNet::ReloadAssembly()
{
    LOG_CORE_INFO("Reloading assembly...");
    Shutdown();
    LoadAssembly();
    LoadProjectAssembly();

    s_Data->AssemblyReloadPending = false;
}

void ScriptingEngineNet::RegisterEntityScript(Entity &entity)
{
    const auto &component = entity.GetComponent<NetScriptComponent>();
    const auto &filepath = AssetManager::GetAsset<NetScript>(component.Handle)->GetPath();
    if (filepath.empty())
    {
        LOG_CORE_ERROR("Skipped .net entity script since it was empty.");
        return;
    }
    if (!std::filesystem::exists(filepath))
    {
        LOG_CORE_ERROR("Skipped .net entity script since it didn't exist.");
        return;
    }

    const auto &className = AssetManager::GetAsset<NetScript>(component.Handle)->GetClassName();
    if (m_GameEntityTypes.find(className) == m_GameEntityTypes.end())
    {
        // The class name parsed in the file was not found in the game's DLL.
        const std::string &msg = "Skipped .net entity script: \n Class: " + className + " not found in " +
                                 std::string(m_GameAssembly.GetName());
        LOG_CORE_ERROR(msg);
        return;
    }

    auto classInstance = m_GameEntityTypes[className]->CreateInstance();

    classInstance.SetPropertyValue("ECSHandle", static_cast<int>(entity.GetEntityID()));
    classInstance.SetPropertyValue("ID", (uint64_t)entity.GetUUID());

    m_EntityToManagedObjects.emplace(entity.GetUUID(), classInstance);
}

Coral::ManagedObject ScriptingEngineNet::GetEntityScript(Entity &entity)
{
    if (m_EntityToManagedObjects.find(entity.GetUUID()) == m_EntityToManagedObjects.end())
    {
        LOG_CRITICAL("Failed to get entity .NET script instance, does not exist");
        throw std::exception("Failed to get entity .Net script instance, doesn't exist");
    }
    return m_EntityToManagedObjects[entity.GetUUID()];
}

bool ScriptingEngineNet::HasEntityScriptInstance(Entity &entity)
{
    return m_EntityToManagedObjects.find(entity.GetUUID()) != m_EntityToManagedObjects.end();
}

void ScriptingEngineNet::GenerateSolution()
{
    // copy engine assembly to project
    const auto scriptModulePath = std::filesystem::current_path() / Project::GetProjectName() / m_NetDirectory;

    // generate premake5 template
    const auto projectName = Project::GetProjectName();
    const std::string premakeScript = R"(
        solution ")" + projectName + R"("
            configurations { "Debug", "Release" }
            platforms { "Any CPU" }
	project ")" + projectName + R"("
	    language "C#"
	    dotnetframework "net8.0"

	    kind "SharedLib"
		clr "Unsafe"

	    -- Don't specify architecture here. (see https://github.com/premake/premake-core/issues/1758)

	    files
	    {
		"../Assets/**.cs"
	    }

	    links
	    {
		"ScriptCore"
	    }
    )";

    // writing premake5 template into file
    const auto premakePath = Project::GetProjectName() + "/" + m_NetDirectory + "/premake5.lua";
    std::ofstream premakeFile(premakePath);
    premakeFile << premakeScript;
    premakeFile.close();

    OS::ExecuteCommand("cd " + scriptModulePath.string() + " && premake5 vs2022");

    OS::OpenIn(scriptModulePath.string() + "/" + projectName + ".sln");
    // delete premake5 template
    std::filesystem::remove(premakePath);
}
} // namespace Engine
