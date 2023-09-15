#include "ScriptEngine.h"

#include "ScriptGlue.h"
#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>

#include <glm/glm.hpp>

#include <fstream>
#include "Log.h"
#include "Components.h"
#include "Project.h"
#include "Application.h"

#include <FileWatch.h>

#define FIELD_ATTRIBUTE_PUBLIC 0x0006

namespace Engine
{
static std::unordered_map<std::string, ScriptFieldType> s_ScriptFieldTypeMap = {
    {"System.Single",  ScriptFieldType::Float},   
	{"System.Double",  ScriptFieldType::Double},
    {"System.Boolean", ScriptFieldType::Bool},   
	{"System.Char",    ScriptFieldType::Char},
    {"System.Int16",   ScriptFieldType::Short},    
	{"System.Int32",   ScriptFieldType::Int},
    {"System.Int64",   ScriptFieldType::Long},     
	{"System.Byte",    ScriptFieldType::Byte},
    {"System.UInt16",  ScriptFieldType::UShort},  
	{"System.UInt32",  ScriptFieldType::UInt},
    {"System.UInt64",  ScriptFieldType::ULong},

    {"Engine.Vector2", ScriptFieldType::Vector2}, 
	{"Engine.Vector3", ScriptFieldType::Vector3},
    {"Engine.Vector4", ScriptFieldType::Vector4},

    {"Engine.Entity",  ScriptFieldType::Entity},
};

namespace Utils
{
static char *ReadBytes(const std::filesystem::path &filepath, uint32_t *outSize)
{
    std::ifstream stream(filepath, std::ios::binary | std::ios::ate);

    if (!stream)
    {
        // Failed to open the file
        return nullptr;
    }

    std::streampos end = stream.tellg();
    stream.seekg(0, std::ios::beg);
    uint32_t size = end - stream.tellg();

    if (size == 0) return nullptr;

    char *buffer = new char[size];
    stream.read((char *)buffer, size);
    stream.close();

    *outSize = size;
    return buffer;
}

static MonoAssembly *LoadMonoAssembly(const std::filesystem::path &assemblyPath)
{
    uint32_t fileSize = 0;
    char *fileData = ReadBytes(assemblyPath, &fileSize);

    // NOTE: We can't use this image for anything other than loading the assembly because this image doesn't have a
    // reference to the assembly
    MonoImageOpenStatus status;
    MonoImage *image = mono_image_open_from_data_full(fileData, fileSize, 1, &status, 0);

    if (status != MONO_IMAGE_OK)
    {
        const char *errorMessage = mono_image_strerror(status);
        // Log some error message using the errorMessage data
        return nullptr;
    }

    MonoAssembly *assembly = mono_assembly_load_from_full(image, assemblyPath.string().c_str(), &status, 0);
    mono_image_close(image);

    // Don't forget to free the file data
    delete[] fileData;

    return assembly;
}

static void PrintAssemblyTypes(MonoAssembly *assembly)
{
    MonoImage *image = mono_assembly_get_image(assembly);
    const MonoTableInfo *typeDefinitionsTable = mono_image_get_table_info(image, MONO_TABLE_TYPEDEF);
    int32_t numTypes = mono_table_info_get_rows(typeDefinitionsTable);

    for (int32_t i = 0; i < numTypes; i++)
    {
        uint32_t cols[MONO_TYPEDEF_SIZE];
        mono_metadata_decode_row(typeDefinitionsTable, i, cols, MONO_TYPEDEF_SIZE);

        const char *nameSpace = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAMESPACE]);
        const char *name = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAME]);

        LOG_CORE_TRACE("{}.{}", nameSpace, name);
    }
}

ScriptFieldType MonoTypeToScriptFieldType(MonoType *monoType)
{
    std::string typeName = mono_type_get_name(monoType);

    auto it = s_ScriptFieldTypeMap.find(typeName);
    if (it == s_ScriptFieldTypeMap.end())
    {
        LOG_CORE_ERROR("Unknown type: {}", typeName);
        return ScriptFieldType::None;
    }

    return it->second;
}
} // namespace Utils

struct ScriptEngineData
{
	MonoDomain *RootDomain = nullptr;
	MonoDomain *AppDomain = nullptr;

	MonoAssembly *CoreAssembly = nullptr;
	MonoImage *CoreAssemblyImage = nullptr;

	MonoAssembly *AppAssembly = nullptr;
	MonoImage *AppAssemblyImage = nullptr;

	std::filesystem::path CoreAssemblyFilepath;
    std::filesystem::path AppAssemblyFilepath;

	ScriptClass EntityClass;

	std::unordered_map<std::string, ScriptClassRef> EntityClasses;
	std::unordered_map<UUID, ScriptInstanceRef> EntityInstances;
    std::unordered_map<UUID, ScriptFieldMap> EntityScriptFields;

	std::unique_ptr<filewatch::FileWatch<std::string>> AppAssemblyFileWatcher;
	bool AssemblyReloadPending = false;

	// Runtime
	Scene *SceneContext = nullptr;
};
static ScriptEngineData* s_Data = nullptr;


static void OnAppAssemblyFileSystemEvent(const std::string &path, const filewatch::Event change_type)
{
    if (!s_Data->AssemblyReloadPending && change_type == filewatch::Event::modified)
    {
		s_Data->AssemblyReloadPending = true;

		Application::SubmitToMainThread(
        []()
        {
            s_Data->AppAssemblyFileWatcher.reset();
            ScriptEngine::ReloadAssembly();
        });
    }
}

void ScriptEngine::Init() 
{
	s_Data = new ScriptEngineData();

	InitMono();
	bool status = LoadAssembly("Resources/Scripts/ScriptCore.dll");
	if (!status)
	{
        LOG_CORE_ERROR("[ScriptEngine] could not load ScriptCore assembly");
		return;
	}

    auto scriptModulePath = Project::GetScriptModulePath();
    status = LoadAppAssembly(scriptModulePath);
    if (!status)
    {
        LOG_CORE_ERROR("[ScriptEngine] Could not load app assembly.");
        return;
    }

	LoadAssemblyClasses();

	ScriptGlue::RegisterFunctions();
	
	s_Data->EntityClass = ScriptClass("Engine", "Entity", true);
    MonoObject *instance = s_Data->EntityClass.Instantiate();
}

void ScriptEngine::Shutdown() 
{
	ShutdownMono();
	delete s_Data;
}

void ScriptEngine::InitMono()
{
    mono_set_assemblies_path("mono/lib");

    MonoDomain *rootDomain = mono_jit_init("EngineJITRuntime");
    if (!rootDomain) return;

    s_Data->RootDomain = rootDomain;
}

bool ScriptEngine::LoadAssembly(const std::filesystem::path &path)
{
    s_Data->AppDomain = mono_domain_create_appdomain((char *)"EngineScriptRuntime", nullptr);
    mono_domain_set(s_Data->AppDomain, true);

	s_Data->CoreAssemblyFilepath = path;
    s_Data->CoreAssembly = Utils::LoadMonoAssembly(path);
    if (s_Data->CoreAssembly == nullptr) return false;

    s_Data->CoreAssemblyImage = mono_assembly_get_image(s_Data->CoreAssembly);
    
	return true;
}

bool ScriptEngine::LoadAppAssembly(const std::filesystem::path &filepath)
{
    s_Data->AppAssemblyFilepath = filepath;
    s_Data->AppAssembly = Utils::LoadMonoAssembly(filepath);
    if (s_Data->AppAssembly == nullptr) return false;

    s_Data->AppAssemblyImage = mono_assembly_get_image(s_Data->AppAssembly);

	s_Data->AppAssemblyFileWatcher = std::make_unique<filewatch::FileWatch<std::string>>(
		filepath.string(),
		OnAppAssemblyFileSystemEvent
	);
	s_Data->AssemblyReloadPending = false;

	return true;
}

ScriptClassRef ScriptEngine::GetEntityClass(const std::string& fullClassName)
{
	if (s_Data->EntityClasses.find(fullClassName) != s_Data->EntityClasses.end())
		return s_Data->EntityClasses[fullClassName];
	else
		return nullptr;
}

std::unordered_map<std::string, ScriptClassRef> ScriptEngine::GetEntityClasses()
{
    return s_Data->EntityClasses; 
}

Scene *ScriptEngine::GetSceneContext() { return s_Data->SceneContext; }

void ScriptEngine::ReloadAssembly()
{
    mono_domain_set(mono_get_root_domain(), false);

    mono_domain_unload(s_Data->AppDomain);

    LoadAssembly(s_Data->CoreAssemblyFilepath);
    LoadAppAssembly(s_Data->AppAssemblyFilepath);
    LoadAssemblyClasses();

    ScriptGlue::RegisterComponents();

    // Retrieve and instantiate class
    s_Data->EntityClass = ScriptClass("Engine", "Entity", true);
}

void ScriptEngine::OnRuntimeStart(Scene *scene) 
{
	s_Data->SceneContext = scene;
}

void ScriptEngine::OnRuntimeStop() 
{
	s_Data->SceneContext = nullptr;
	s_Data->EntityInstances.clear();
}

bool ScriptEngine::EntityClassExists(const std::string &fullClassName)
{
	return s_Data->EntityClasses.find(fullClassName) != s_Data->EntityClasses.end();
}

void ScriptEngine::OnCreateEntity(Entity entity) 
{
    const auto &sc = entity.GetComponent<ScriptComponent>();
    if (ScriptEngine::EntityClassExists(sc.ClassName))
    {
		UUID entityID = entity.GetComponent<IDComponent>().ID;

		ScriptInstanceRef instance = std::make_shared<ScriptInstance>(s_Data->EntityClasses[sc.ClassName], entity);
		s_Data->EntityInstances[entityID] = instance;

		// Copy field values
        if (s_Data->EntityScriptFields.find(entityID) != s_Data->EntityScriptFields.end())
        {
            const ScriptFieldMap &fieldMap = s_Data->EntityScriptFields.at(entityID);
            for (const auto &[name, fieldInstance] : fieldMap)
                instance->SetFieldValueInternal(name, fieldInstance.m_Buffer);
        }

		instance->InvokeOnCreate();
    }
}

void ScriptEngine::OnUpdateEntity(Entity entity, float dt) 
{
    UUID entityUUID = entity.GetComponent<IDComponent>().ID;
    if (s_Data->EntityInstances.find(entityUUID) != s_Data->EntityInstances.end())
    {
        ScriptInstanceRef instance = s_Data->EntityInstances[entityUUID];
        instance->InvokeOnUpdate(dt);
    }
    else
    {
		LOG_CORE_ERROR("Could not find ScriptInstance for entity {}", entityUUID);
    }
}

MonoObject *ScriptEngine::InstantiateClass(MonoClass *monoClass) 
{
	MonoObject *instance = mono_object_new(s_Data->AppDomain, monoClass);
	mono_runtime_object_init(instance);
        
	return instance;
}

ScriptInstanceRef ScriptEngine::GetEntityScriptInstance(UUID entityID)
{
	if (s_Data->EntityInstances.find(entityID) != s_Data->EntityInstances.end())
		return s_Data->EntityInstances[entityID];
	else
		return nullptr;
}

ScriptFieldMap &ScriptEngine::GetScriptFieldMap(Entity entity)
{
    assert(entity);

    UUID entityID = entity.GetComponent<IDComponent>().ID;
    return s_Data->EntityScriptFields[entityID];
}

void ScriptEngine::LoadAssemblyClasses()
{
	s_Data->EntityClasses.clear();

	const MonoTableInfo *typeDefinitionsTable = mono_image_get_table_info(s_Data->AppAssemblyImage, MONO_TABLE_TYPEDEF);
	int32_t numTypes = mono_table_info_get_rows(typeDefinitionsTable);
    MonoClass *entityClass = mono_class_from_name(s_Data->CoreAssemblyImage, "Engine", "Entity");

	for (int32_t i = 0; i < numTypes; i++)
	{
		uint32_t cols[MONO_TYPEDEF_SIZE];
		mono_metadata_decode_row(typeDefinitionsTable, i, cols, MONO_TYPEDEF_SIZE);

		const char *nameSpace = mono_metadata_string_heap(s_Data->AppAssemblyImage, cols[MONO_TYPEDEF_NAMESPACE]);
		const char *name = mono_metadata_string_heap(s_Data->AppAssemblyImage, cols[MONO_TYPEDEF_NAME]);
        std::string fullName;
		fullName = strlen(nameSpace) > 0 ? fmt::format("{}.{}", nameSpace, name) : name;

		MonoClass *monoClass = mono_class_from_name(s_Data->AppAssemblyImage, nameSpace, name);

		if (monoClass == entityClass) continue;

		bool isEntity = mono_class_is_subclass_of(monoClass, entityClass, false);
		if (!isEntity) continue;
		
		ScriptClassRef scriptClass = std::make_shared<ScriptClass>(nameSpace, name, false);
		s_Data->EntityClasses[fullName] = scriptClass;
		
		// This routine is an iterator routine for retrieving the fields in a class.
        // You must pass a gpointer that points to zero and is treated as an opaque handle
        // to iterate over all of the elements. When no more values are available, the return value is NULL.

        int fieldCount = mono_class_num_fields(monoClass);
        LOG_CORE_WARN("{} has {} fields:", name, fieldCount);
        void *iterator = nullptr;
        while (MonoClassField *field = mono_class_get_fields(monoClass, &iterator))
        {
            const char *fieldName = mono_field_get_name(field);
            uint32_t flags = mono_field_get_flags(field);
            if (flags & FIELD_ATTRIBUTE_PUBLIC)
            {
                MonoType *type = mono_field_get_type(field);
                ScriptFieldType fieldType = Utils::MonoTypeToScriptFieldType(type);
                LOG_CORE_WARN("  {} ({})", fieldName, Utils::ScriptFieldTypeToString(fieldType));

                scriptClass->m_Fields[fieldName] = {fieldType, fieldName, field};
            }
        }

		LOG_CORE_TRACE("{}.{}", nameSpace, name);
	}
}

MonoImage *ScriptEngine::GetCoreAssemblyImage() { return s_Data->CoreAssemblyImage; }

void ScriptEngine::ShutdownMono()
{
	mono_domain_unload(s_Data->AppDomain);
	mono_jit_cleanup(s_Data->RootDomain);
}

ScriptClass::ScriptClass(const std::string &classNamespace, const std::string &className, bool isCore)
    : m_ClassNamespace(classNamespace), m_ClassName(className)
{
    m_MonoClass = mono_class_from_name(isCore ? s_Data->CoreAssemblyImage : s_Data->AppAssemblyImage,
                                        classNamespace.c_str(), className.c_str());
}

MonoObject *ScriptClass::Instantiate() { return ScriptEngine::InstantiateClass(m_MonoClass); }

MonoMethod *ScriptClass::GetMethod(const std::string &methodName, int parameterCount) 
{
    return mono_class_get_method_from_name(m_MonoClass, methodName.c_str(), parameterCount);
}

MonoObject *ScriptClass::InvokeMethod(MonoObject *instance, MonoMethod *method, void **params)
{
	return mono_runtime_invoke(method, instance, params, nullptr);
}

ScriptInstance::ScriptInstance(ScriptClassRef scriptClass, Entity entity) : m_ScriptClass(scriptClass) 
{
    m_Instance = scriptClass->Instantiate();

    m_Constructor = s_Data->EntityClass.GetMethod(".ctor", 1);
    m_OnCreateMethod = scriptClass->GetMethod("OnCreate", 0);
    m_OnUpdateMethod = scriptClass->GetMethod("OnUpdate", 1);

    // Call Entity constructor
    {
        UUID entityID = entity.GetComponent<IDComponent>().ID;
        void *param = &entityID;
        m_ScriptClass->InvokeMethod(m_Instance, m_Constructor, &param);
    }
}

void ScriptInstance::InvokeOnCreate() 
{
    if (m_OnCreateMethod)
		m_ScriptClass->InvokeMethod(m_Instance, m_OnCreateMethod);
}

void ScriptInstance::InvokeOnUpdate(float dt) 
{
    if (m_OnUpdateMethod)
    {
        void *args[1];
        args[0] = &dt;
        m_ScriptClass->InvokeMethod(m_Instance, m_OnUpdateMethod, args);
    }
}

bool ScriptInstance::GetFieldValueInternal(const std::string &name, void *buffer)
{
    const auto &fields = m_ScriptClass->GetFields();
    auto it = fields.find(name);
    if (it == fields.end()) return false;

    const ScriptField &field = it->second;
    mono_field_get_value(m_Instance, field.ClassField, buffer);
    return true;
}

bool ScriptInstance::SetFieldValueInternal(const std::string &name, const void *value)
{
    const auto &fields = m_ScriptClass->GetFields();
    auto it = fields.find(name);
    if (it == fields.end()) return false;

    const ScriptField &field = it->second;
    mono_field_set_value(m_Instance, field.ClassField, (void *)value);
    return true;
}
} // namespace Engine
