#include "LuaScriptEngine.h"

extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}
#include <LuaBridge/LuaBridge.h>

#include <iostream>
#include <unordered_map>

namespace Engine
{
auto LuaScriptEngine::s_Functions = std::unordered_map<std::string, std::any>();

// clang-format off
static const char *s_KeyboardInput[] = {
    "A","C","D","E","F","B","G","H","I","J","K","L","M","N","O","P","Q","R","S","T","U","V","W","X","Y","Z",
    "Space","Escape","Enter","Left","Right","Up","Down","LeftShift","RightShift","LeftControl","RightControl",
    "LeftAlt","RightAlt","LeftSuper","RightSuper","Tab","Backspace","Insert","Delete","PageUp","PageDown","Home","End",
};
// clang-format on

static const char *s_MouseButtons[] = {"Left", "Right", "Middle"};

struct ScriptData
{
    std::string Name;
    std::string Filepath;
    lua_State *LuaState;
};
static ScriptData s_ScriptData;

void printMessage(const std::string &s) { std::cout << s << std::endl; }
void testFunction() { std::cout << "testFunction" << std::endl; }

static luabridge::LuaRef GetGlobalFunction(lua_State *L, const std::string &name)
{
    luabridge::LuaRef global = luabridge::getGlobal(L, name.c_str());
    if (global.isFunction()) return global;
    return luabridge::LuaRef(L);
}

CallFunctions LuaScriptEngine::LoadScript(const std::string &filepath, const std::string &name)
{
    UnloadScript(name);

    s_ScriptData.Name = name;
    s_ScriptData.Filepath = filepath;
    s_ScriptData.LuaState = luaL_newstate();

    luaL_openlibs(s_ScriptData.LuaState);

    // register functions
    for (const auto &function : s_Functions)
    {
        luabridge::getGlobalNamespace(s_ScriptData.LuaState)
            .addFunction(function.first.c_str(), std::any_cast<std::function<void()>>(function.second));
    }

    CallFunctions callFunctions;

    if (CheckLua(luaL_dofile(s_ScriptData.LuaState, filepath.c_str())))
    {
        auto &L = s_ScriptData.LuaState;

        auto onCreate = GetGlobalFunction(L, "OnCreate");
        if (onCreate.isFunction()) callFunctions.OnCreate = onCreate;
        auto onUpdate = GetGlobalFunction(L, "OnUpdate");
        if (onUpdate.isFunction()) callFunctions.OnUpdate = onUpdate;
        auto OnDestroy = GetGlobalFunction(L, "OnDestroy");
        if (OnDestroy.isFunction()) callFunctions.OnDestroy = OnDestroy;

        auto onKeyPressed = GetGlobalFunction(L, "OnKeyPressed");
        if (onKeyPressed.isFunction())
        {
            callFunctions.OnKeyPressed = [onKeyPressed](InputKey key, bool isRepeat)
            { onKeyPressed(s_KeyboardInput[static_cast<int>(key)], isRepeat); };
        }
        auto onKeyReleased = GetGlobalFunction(L, "OnKeyReleased");
        if (onKeyReleased.isFunction())
        {
            callFunctions.OnKeyReleased = [onKeyReleased](InputKey key)
            { onKeyReleased(s_KeyboardInput[static_cast<int>(key)]); };
        }
        auto onMousePressed = GetGlobalFunction(L, "OnMouseButtonPressed");
        if (onMousePressed.isFunction())
        {
            callFunctions.OnMouseButtonPressed = [onMousePressed](MouseButton button)
            { onMousePressed(s_MouseButtons[static_cast<int>(button)]); };
        }
        auto onMouseReleased = GetGlobalFunction(L, "OnMouseButtonReleased");
        if (onMouseReleased.isFunction())
        {
            callFunctions.OnMouseButtonReleased = [onMouseReleased](MouseButton button)
            { onMouseReleased(s_MouseButtons[static_cast<int>(button)]); };
        }
        auto onMouseMoved = GetGlobalFunction(L, "OnMouseMoved");
        if (onMouseMoved.isFunction())
        {
            callFunctions.OnMouseMoved = [onMouseMoved](float x, float y, float xOffset, float yOffset)
            { onMouseMoved(x, y, xOffset, yOffset); };
        }
        auto onMouseScrolled = GetGlobalFunction(L, "OnMouseScrolled");
        if (onMouseScrolled.isFunction())
        {
            callFunctions.OnMouseScrolled = [onMouseScrolled](float x, float y) { onMouseScrolled(x, y); };
        }
    }
    return callFunctions;
}

void LuaScriptEngine::UnloadScript(const std::string &name)
{
    // lua_close(s_ScriptData.LuaState);
    if (s_ScriptData.Name == name)
    {
        s_ScriptData.Name = "";
        s_ScriptData.Filepath = "";
        s_ScriptData.LuaState = nullptr;
        s_Functions.clear();
    }
}

bool LuaScriptEngine::CheckLua(int r)
{
    if (r != LUA_OK)
    {
        std::cout << "Error: " << lua_tostring(s_ScriptData.LuaState, -1) << std::endl;
        return false;
    }
    return true;
}
} // namespace Engine