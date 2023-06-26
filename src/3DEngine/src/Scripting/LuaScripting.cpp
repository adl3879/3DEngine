#include "LuaScripting.h"

#define SOL_ALL_SAFETIES_ON 1
#include <sol/sol.hpp>
#include "Components.h"

#include <map>

namespace Engine
{

static void AddInputFunctions(sol::state &luaState);

// clang-format off
static const char *s_KeyboardInput[] = {
    "A","B","C","D","E","F","G","H","I","J","K","L","M","N","O","P","Q","R","S","T","U","V","W","X","Y","Z",
    "Space","Escape","Enter","Left","Right","Up","Down","LeftShift","RightShift","LeftControl","RightControl",
    "LeftAlt","RightAlt","LeftSuper","RightSuper","Tab","Backspace","Insert","Delete","PageUp","PageDown","Home","End",
};
// turn to map with value as integer
static std::map<std::string, int> s_KeyboardInputMap = {
    {"A", 0}, {"B", 1}, {"C", 2}, {"D", 3}, {"E", 4}, {"F", 5}, {"G", 6}, {"H", 7}, {"I", 8}, {"J", 9}, {"K", 10},
    {"L", 11}, {"M", 12}, {"N", 13}, {"O", 14}, {"P", 15}, {"Q", 16}, {"R", 17}, {"S", 18}, {"T", 19}, {"U", 20},
    {"V", 21}, {"W", 22}, {"X", 23}, {"Y", 24}, {"Z", 25}, {"Space", 26}, {"Escape", 27}, {"Enter", 28}, {"Left", 29},
    {"Right", 30}, {"Up", 31}, {"Down", 32}, {"LeftShift", 33}, {"RightShift", 34}, {"LeftControl", 35},
    {"RightControl", 36}, {"LeftAlt", 37}, {"RightAlt", 38}, {"LeftSuper", 39}, {"RightSuper", 40}, {"Tab", 41},
    {"Backspace", 42}, {"Insert", 43}, {"Delete", 44}, {"PageUp", 45}, {"PageDown", 46}, {"Home", 47}, {"End", 48},
};
// clang-format on

static const char *s_MouseButtons[] = {"Left", "Right", "Middle"};
static std::map<std::string, int> s_MouseButtonsMap = {{"Left", 0}, {"Right", 1}, {"Middle", 2}};

LuaScriptInstance::LuaScriptInstance(const std::string &filepath, const std::string &name) : m_Filepath(filepath)
{
    m_LuaState = new sol::state();
    m_LuaState->open_libraries(sol::lib::base, sol::lib::package, sol::lib::math, sol::lib::string, sol::lib::table,
                               sol::lib::debug, sol::lib::io, sol::lib::os, sol::lib::coroutine);

    // open file
    m_LuaState->script_file(filepath);

    // get callback functions
    m_CallbackFunctions.OnCreate = m_LuaState->get<sol::function>("OnCreate");
    m_CallbackFunctions.OnDestroy = m_LuaState->get<sol::function>("OnDestroy");
    m_CallbackFunctions.OnUpdate = m_LuaState->get<sol::function>("OnUpdate");

    // input callback functions
    m_CallbackFunctions.OnKeyPressed = [this](InputKey key, bool repeat)
    {
        if (m_LuaState->get<sol::function>("OnKeyPressed").valid())
            m_LuaState->get<sol::function>("OnKeyPressed")(s_KeyboardInput[static_cast<int>(key)], repeat);
    };
    m_CallbackFunctions.OnKeyReleased = [this](InputKey key)
    {
        if (m_LuaState->get<sol::function>("OnKeyReleased").valid())
            m_LuaState->get<sol::function>("OnKeyReleased")(s_KeyboardInput[static_cast<int>(key)]);
    };
    m_CallbackFunctions.OnMouseButtonPressed = [this](MouseButton button)
    {
        if (m_LuaState->get<sol::function>("OnMouseButtonPressed").valid())
            m_LuaState->get<sol::function>("OnMouseButtonPressed")(s_MouseButtons[static_cast<int>(button)]);
    };
    m_CallbackFunctions.OnMouseButtonReleased = [this](MouseButton button)
    {
        if (m_LuaState->get<sol::function>("OnMouseButtonReleased").valid())
            m_LuaState->get<sol::function>("OnMouseButtonReleased")(s_MouseButtons[static_cast<int>(button)]);
    };
    m_CallbackFunctions.OnMouseMoved = m_LuaState->get<sol::function>("OnMouseMoved");
    m_CallbackFunctions.OnMouseScrolled = m_LuaState->get<sol::function>("OnMouseScrolled");
}

static void AddInputFunctions(sol::state &luaState)
{
    auto inputTable = luaState.create_table("Input");

    inputTable.set_function(
        "IsKeyPressed",
        [](std::string key) -> bool
        { return InputManager::Instance().IsKeyPressed(static_cast<InputKey>(s_KeyboardInputMap[key])); });
    inputTable.set_function(
        "IsMouseButtonPressed",
        [](std::string button) -> bool
        { return InputManager::Instance().IsMouseButtonPressed(static_cast<MouseButton>(s_MouseButtonsMap[button])); });
}

void LuaScriptInstance::Setup()
{
    // add functions to the Input namespace
    AddInputFunctions(*m_LuaState);
}

void LuaScriptInstance::ReloadScriptIfModified()
{
    fs::file_time_type currentWriteTime = fs::last_write_time(m_Filepath);
    if (currentWriteTime != m_LastWriteTime)
    {
        m_LastWriteTime = currentWriteTime;
        m_LuaState->script_file(m_Filepath);
        Setup();
    }
}

LuaScriptInstance::~LuaScriptInstance()
{
    delete m_LuaState;
    m_LuaState = nullptr;
}
} // namespace Engine