#include "ScriptableEntity.h"

#include <iostream>

namespace Engine
{
static void printMessage(const std::string &s) { std::cout << s << std::endl; }

LuaScriptableEntity::LuaScriptableEntity(const std::string &filepath, const std::string &name)
    : m_Filepath(filepath), m_Name(name)
{
    // LuaScriptEngine::RegisterFunction("printMessage", printMessage);

    m_Script = LuaScriptEngine::LoadScript(filepath, name);

    // register input callbacks
    InputManager::Instance().RegisterKeyboardCallback(
        std::bind(&LuaScriptableEntity::OnKeyPressed, this, std::placeholders::_1, std::placeholders::_2));
    InputManager::Instance().RegisterKeyReleasedCallback(
        std::bind(&LuaScriptableEntity::OnKeyReleased, this, std::placeholders::_1));
    InputManager::Instance().RegisterMousePressedCallback(
        std::bind(&LuaScriptableEntity::OnMouseButtonPressed, this, std::placeholders::_1));
    InputManager::Instance().RegisterMouseReleasedCallback(
        std::bind(&LuaScriptableEntity::OnMouseButtonReleased, this, std::placeholders::_1));
    InputManager::Instance().RegisterMouseScrollCallback(
        std::bind(&LuaScriptableEntity::OnMouseScrolled, this, std::placeholders::_1, std::placeholders::_2));
    InputManager::Instance().RegisterMouseMovedCallback(std::bind(&LuaScriptableEntity::OnMouseMoved, this,
                                                                  std::placeholders::_1, std::placeholders::_2,
                                                                  std::placeholders::_3, std::placeholders::_4));
}
void LuaScriptableEntity::OnCreate() { m_Script.OnCreate(); }
void LuaScriptableEntity::OnDestroy() { m_Script.OnDestroy(); }
void LuaScriptableEntity::OnUpdate(float dt) { m_Script.OnUpdate(dt); }
} // namespace Engine