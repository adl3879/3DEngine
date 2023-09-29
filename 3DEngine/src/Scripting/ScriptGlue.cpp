#include "ScriptGlue.h"

#include <mono/metadata/object.h>
#include <mono/metadata/reflection.h>
#include <glm/glm.hpp>

#include "Log.h"
#include "UUID.h"
#include "AllComponents.h"
#include "ScriptEngine.h"
#include "Input.h"

namespace Engine
{
#define ADD_INTERNAL_CALL(name) mono_add_internal_call("Engine.InternalCalls::" #name, (const void *)name) 

static std::unordered_map<MonoType *, std::function<bool(Entity)>> s_EntityHasComponentFuncs;

static void NativeLog(MonoString *string, int parameter)
{
    char *str = mono_string_to_utf8(string);
    LOG_CORE_TRACE("{}, {}", str, parameter);
    mono_free(str);
}

static void NativeLog_Vector(glm::vec3 *parameter, glm::vec3 *out)
{
    LOG_CORE_TRACE("x: {}, y: {}, z: {}", parameter->x, parameter->y, parameter->z);
    *out = glm::normalize(*parameter);
}

static bool Entity_HasComponent(UUID entityID, MonoReflectionType *componentType)
{
    Scene *scene = ScriptEngine::GetSceneContext();
    assert(scene);
    Entity entity = scene->GetEntityByUUID(entityID);
    assert(entity);

    MonoType *managedType = mono_reflection_type_get_type(componentType);
    assert(s_EntityHasComponentFuncs.find(managedType) != s_EntityHasComponentFuncs.end());
    return s_EntityHasComponentFuncs.at(managedType)(entity);
}

static uint64_t Entity_FindEntityByName(MonoString *name)
{
    char *nameCStr = mono_string_to_utf8(name);

    Scene *scene = ScriptEngine::GetSceneContext();
    assert(scene);
    Entity entity = scene->FindEntityByName(nameCStr);
    mono_free(nameCStr);

    if (!entity) return 0;

    return entity.GetComponent<IDComponent>().ID;
}

static void TransformComponent_GetTranslation(UUID entityID, glm::vec3 *outTranslation)
{
    Scene *scene = ScriptEngine::GetSceneContext();
    assert(scene);
    Entity entity = scene->GetEntityByUUID(entityID);
    assert(entity);

    *outTranslation = entity.GetComponent<TransformComponent>().Translation;
}

static void TransformComponent_SetTranslation(UUID entityID, glm::vec3 *translation)
{
    Scene *scene = ScriptEngine::GetSceneContext();
    assert(scene);
    Entity entity = scene->GetEntityByUUID(entityID);
	assert(entity);

    entity.GetComponent<TransformComponent>().Translation = *translation;
}

static bool Input_IsKeyDown(int keycode)
{
	return InputManager::Instance().IsKeyPressed(static_cast<InputKey>(keycode));
}

template <typename... Component> static void RegisterComponent()
{
    (
        []()
        {
            std::string_view typeName = typeid(Component).name();
            size_t pos = typeName.find_last_of(':');
            std::string_view structName = typeName.substr(pos + 1);
            std::string managedTypename = fmt::format("Hazel.{}", structName);

            MonoType *managedType =
                mono_reflection_type_from_name(managedTypename.data(), ScriptEngine::GetCoreAssemblyImage());
            if (!managedType)
            {
                LOG_CORE_ERROR("Could not find component type {}", managedTypename);
                return;
            }
            s_EntityHasComponentFuncs[managedType] = [](Entity entity) { return entity.HasComponent<Component>(); };
        }(),
        ...);
}

template <typename... Component> static void RegisterComponent(ComponentGroup<Component...>)
{
    RegisterComponent<Component...>();
}

void ScriptGlue::RegisterComponents() 
{
    s_EntityHasComponentFuncs.clear();
    RegisterComponent(AllComponents{});
}

void ScriptGlue::RegisterFunctions()
{ 
	ADD_INTERNAL_CALL(NativeLog);
	ADD_INTERNAL_CALL(NativeLog_Vector);
	ADD_INTERNAL_CALL(Entity_HasComponent);
	ADD_INTERNAL_CALL(Entity_FindEntityByName);
	ADD_INTERNAL_CALL(TransformComponent_GetTranslation);
	ADD_INTERNAL_CALL(TransformComponent_SetTranslation);
	ADD_INTERNAL_CALL(Input_IsKeyDown);
}
}
