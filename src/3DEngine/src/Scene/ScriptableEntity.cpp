#include "ScriptableEntity.h"

#include <iostream>

#include <sol/sol.hpp>
#include "Components.h"
#include "PlatformUtils.h"

namespace Engine
{
// static void printMessage(const std::string &s) { std::cout << s << std::endl; }

LuaScriptableEntity::LuaScriptableEntity(const std::string &filepath, const std::string &name)
    : m_Filepath(filepath), m_Name(name)
{
    auto path = Utils::Path::GetAbsolute(filepath);
    m_Script = std::make_shared<LuaScriptInstance>(path, name);

    // register callbacks
    const auto &callbackFuncs = m_Script->GetCallbackFunctions();

    InputManager::Instance().RegisterKeyboardCallback(callbackFuncs.OnKeyPressed);
    InputManager::Instance().RegisterKeyReleasedCallback(callbackFuncs.OnKeyReleased);
    InputManager::Instance().RegisterMousePressedCallback(callbackFuncs.OnMouseButtonPressed);
    InputManager::Instance().RegisterMouseReleasedCallback(callbackFuncs.OnMouseButtonReleased);
    InputManager::Instance().RegisterMouseMovedCallback(callbackFuncs.OnMouseMoved);
    InputManager::Instance().RegisterMouseScrollCallback(callbackFuncs.OnMouseScrolled);
}

// clang-format off
void LuaScriptableEntity::Setup()
{
    auto s = static_cast<sol::state *>(m_Script->m_LuaState);

    s->set_function("printMessage", [](const std::string &s) { std::cout << s << rand() << std::endl; });
    auto &transform = m_Entity.GetComponent<TransformComponent>();


    struct Point3
    {
        float x, y, z;
    };
    struct Transform
    {
        Point3 Translation, Rotation, Scale;
    };
    auto entityTable = s->create_table("entity");

    entityTable.new_usertype<Point3>("Point3", "x", &Point3::x, "y", &Point3::y, "z", &Point3::z);
    entityTable.new_usertype<Transform>("Transform", "Translation", &Transform::Translation, "Rotation", &Transform::Rotation,
                               "Scale", &Transform::Scale);
    entityTable.set_function("getTransform",
    [&transform]() -> Transform {
        return {
            .Translation = {transform.Translation.x, transform.Translation.y, transform.Translation.z},
            .Rotation = {transform.Rotation.x, transform.Rotation.y, transform.Rotation.z},
            .Scale = {transform.Scale.x, transform.Scale.y, transform.Scale.z},
        };
    });
    entityTable.set_function("setTransform", 
    [&transform](std::string type, float x, float y, float z) {
        if (type == "Translation") transform.Translation = {x, y, z};
        else if (type == "Rotation") transform.Rotation = {x, y, z};
        else if (type == "Scale") transform.Scale = {x, y, z};
    });

    m_Script->Setup();
}
} // namespace Engine