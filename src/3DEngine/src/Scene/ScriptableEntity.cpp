#include "ScriptableEntity.h"

#include <iostream>

namespace Engine
{
static void printMessage(const std::string &s) { std::cout << s << std::endl; }

LuaScriptableEntity::LuaScriptableEntity(const std::string &filepath, const std::string &name)
    : m_Filepath(filepath), m_Name(name)
{
    m_Script = std::make_shared<LuaScriptInstance>(filepath, name);

    // register functions
    m_Script->RegisterFunction1S("printMessage", &printMessage);

    // register callbacks
    const auto &callbackFuncs = m_Script->GetCallbackFunctions();

    InputManager::Instance().RegisterKeyboardCallback(callbackFuncs.OnKeyPressed);
    InputManager::Instance().RegisterKeyReleasedCallback(callbackFuncs.OnKeyReleased);
    InputManager::Instance().RegisterMousePressedCallback(callbackFuncs.OnMouseButtonPressed);
    InputManager::Instance().RegisterMouseReleasedCallback(callbackFuncs.OnMouseButtonReleased);
    InputManager::Instance().RegisterMouseMovedCallback(callbackFuncs.OnMouseMoved);
    InputManager::Instance().RegisterMouseScrollCallback(callbackFuncs.OnMouseScrolled);
}
} // namespace Engine