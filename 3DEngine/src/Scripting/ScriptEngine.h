#pragma once

#include <string>
#include <filesystem>
#include <memory>
#include <unordered_map>

#include "Scene.h"
#include "Entity.h"

extern "C" {
	typedef struct _MonoClass MonoClass;
	typedef struct _MonoObject MonoObject;
	typedef struct _MonoMethod MonoMethod;
	typedef struct _MonoAssembly MonoAssembly;
	typedef struct _MonoImage MonoImage;
	typedef struct _MonoClassField MonoClassField;
}

namespace Engine
{
enum class ScriptFieldType
{
    None = 0,
    Float,
    Double,
    Bool,
    Char,
    Byte,
    Short,
    Int,
    Long,
    UByte,
    UShort,
    UInt,
    ULong,
    Vector2,
    Vector3,
    Vector4,
    Entity
};

struct ScriptField
{
    ScriptFieldType Type;
    std::string Name;

    MonoClassField *ClassField;
};

// ScriptField + data storage
struct ScriptFieldInstance
{
	ScriptField Field;

	ScriptFieldInstance()
	{
		memset(m_Buffer, 0, sizeof(m_Buffer));
	}

	template<typename T>
	T GetValue()
	{
		static_assert(sizeof(T) <= 16, "Type too large!");
		return *(T*)m_Buffer;
	}

	template<typename T>
	void SetValue(T value)
	{
		static_assert(sizeof(T) <= 16, "Type too large!");
		memcpy(m_Buffer, &value, sizeof(T));
	}
  private:
	uint8_t m_Buffer[16];

	friend class ScriptEngine;
	friend class ScriptInstance;
};
using ScriptFieldMap = std::unordered_map<std::string, ScriptFieldInstance>;

class ScriptClass
{
  public:
    ScriptClass() = default;
	ScriptClass(const std::string &classNamespace, const std::string &className, bool isCore);

    MonoObject *Instantiate();

	MonoMethod *GetMethod(const std::string &methodName, int parameterCount);
	MonoObject *InvokeMethod(MonoObject *instance, MonoMethod *method, void **params = nullptr);

	MonoClass *GetMonoClass() const { return m_MonoClass; }
	const std::map<std::string, ScriptField> &GetFields() const { return m_Fields; }

  private:
    std::string m_ClassNamespace;
    std::string m_ClassName;

	std::map<std::string, ScriptField> m_Fields;

    MonoClass *m_MonoClass = nullptr;

	friend class ScriptEngine;
};
using ScriptClassRef = std::shared_ptr<ScriptClass>;

class ScriptInstance
{
  public:
    ScriptInstance(ScriptClassRef scriptClass, Entity entity);

	void InvokeOnCreate();
	void InvokeOnUpdate(float dt);

	template <typename T> T GetFieldValue(const std::string &name)
    {
        static_assert(sizeof(T) <= 16, "Type too large!");

        bool success = GetFieldValueInternal(name, s_FieldValueBuffer);
        if (!success) return T();

        return *(T *)s_FieldValueBuffer;
    }

    template <typename T> void SetFieldValue(const std::string &name, T value)
    {
        static_assert(sizeof(T) <= 16, "Type too large!");

        SetFieldValueInternal(name, &value);
    }

	ScriptClassRef GetScriptClass() const { return m_ScriptClass; }

  private:
    bool GetFieldValueInternal(const std::string &name, void *buffer);
    bool SetFieldValueInternal(const std::string &name, const void *value);

  private:
	ScriptClassRef m_ScriptClass;

	MonoObject *m_Instance = nullptr;
    MonoMethod *m_Constructor = nullptr;
	MonoMethod *m_OnCreateMethod = nullptr;
	MonoMethod *m_OnUpdateMethod = nullptr;

	inline static char s_FieldValueBuffer[16];

	friend class ScriptEngine;
};
using ScriptInstanceRef = std::shared_ptr<ScriptInstance>;

class ScriptEngine
{
  public:
	static void Init();
	static void Shutdown();

	static void ReloadAssembly();
	
	static bool LoadAssembly(const std::filesystem::path &path);
    static bool LoadAppAssembly(const std::filesystem::path &path);

	static ScriptClassRef GetEntityClass(const std::string &fullClassName);
    static std::unordered_map<std::string, ScriptClassRef> GetEntityClasses();
    static Scene *GetSceneContext();
    static ScriptFieldMap &GetScriptFieldMap(Entity entity);
	static ScriptInstanceRef GetEntityScriptInstance(UUID entityID);

	static void OnRuntimeStart(Scene *scene);
    static void OnRuntimeStop();

	static bool EntityClassExists(const std::string &fullClassName);
    static void OnCreateEntity(Entity entity);
    static void OnUpdateEntity(Entity entity, float dt);

	static MonoImage *GetCoreAssemblyImage();
  private:
	static void InitMono();
	static void ShutdownMono();

    
	static MonoObject *InstantiateClass(MonoClass *monoClass);
	static void LoadAssemblyClasses();

	friend class ScriptClass;
    friend class ScriptGlue;
};

namespace Utils
{

inline const char *ScriptFieldTypeToString(ScriptFieldType fieldType)
{
    switch (fieldType)
    {
        case ScriptFieldType::None:		return "None";
        case ScriptFieldType::Float:	return "Float";
        case ScriptFieldType::Double:	return "Double";
        case ScriptFieldType::Bool:		return "Bool";
        case ScriptFieldType::Char:		return "Char";
        case ScriptFieldType::Byte:		return "Byte";
        case ScriptFieldType::Short:	return "Short";
        case ScriptFieldType::Int:		return "Int";
        case ScriptFieldType::Long:		return "Long";
        case ScriptFieldType::UByte:	return "UByte";
        case ScriptFieldType::UShort:	return "UShort";
        case ScriptFieldType::UInt:		return "UInt";
        case ScriptFieldType::ULong:	return "ULong";
        case ScriptFieldType::Vector2:	return "Vector2";
        case ScriptFieldType::Vector3:	return "Vector3";
        case ScriptFieldType::Vector4:	return "Vector4";
        case ScriptFieldType::Entity:	return "Entity";
    }
    assert(false);
    return "None";
}

inline ScriptFieldType ScriptFieldTypeFromString(std::string_view fieldType)
{
    if (fieldType == "None")	return ScriptFieldType::None;
    if (fieldType == "Float")	return ScriptFieldType::Float;
    if (fieldType == "Double")	return ScriptFieldType::Double;
    if (fieldType == "Bool")	return ScriptFieldType::Bool;
    if (fieldType == "Char")	return ScriptFieldType::Char;
    if (fieldType == "Byte")	return ScriptFieldType::Byte;
    if (fieldType == "Short")	return ScriptFieldType::Short;
    if (fieldType == "Int")		return ScriptFieldType::Int;
    if (fieldType == "Long")	return ScriptFieldType::Long;
    if (fieldType == "UByte")	return ScriptFieldType::UByte;
    if (fieldType == "UShort")	return ScriptFieldType::UShort;
    if (fieldType == "UInt")	return ScriptFieldType::UInt;
    if (fieldType == "ULong")	return ScriptFieldType::ULong;
    if (fieldType == "Vector2") return ScriptFieldType::Vector2;
    if (fieldType == "Vector3") return ScriptFieldType::Vector3;
    if (fieldType == "Vector4") return ScriptFieldType::Vector4;
    if (fieldType == "Entity")	return ScriptFieldType::Entity;

    assert(false);
    return ScriptFieldType::None;
}
} // namespace Utils
} // namespace Engine
