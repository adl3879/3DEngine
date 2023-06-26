#pragma once

#include <string>
#include <iostream>
#include <any>

#include "InputManager.h"
#include "Entity.h"

#include <filesystem>

namespace fs = std::filesystem;

namespace sol
{
class state;
} // namespace sol

namespace Engine
{
struct LuaCallbackFunctions
{
    std::function<void()> OnCreate = []() {};
    std::function<void()> OnDestroy = []() {};
    std::function<void(float)> OnUpdate = [](float dt) {};

    std::function<void(InputKey key, bool isRepeat)> OnKeyPressed = [](InputKey key, bool isRepeat) {};
    std::function<void(InputKey key)> OnKeyReleased = [](InputKey key) {};
    std::function<void(MouseButton button)> OnMouseButtonPressed = [](MouseButton button) {};
    std::function<void(MouseButton button)> OnMouseButtonReleased = [](MouseButton button) {};
    std::function<void(double xPos, double yPos, double xOffset, double yOffset)> OnMouseMoved =
        [](double xPos, double yPos, double xOffset, double yOffset) {};
    std::function<void(double xOffset, double yOffset)> OnMouseScrolled = [](double xOffset, double yOffset) {};
};

class LuaScriptInstance
{
  public:
    LuaScriptInstance(const std::string &filepath, const std::string &name);
    ~LuaScriptInstance();

    void Setup();
    void ReloadScriptIfModified();

  public:
    const LuaCallbackFunctions &GetCallbackFunctions() const { return m_CallbackFunctions; }

  private:
    sol::state *m_LuaState = nullptr;
    LuaCallbackFunctions m_CallbackFunctions;

    friend class LuaScriptableEntity;

  private:
    std::string m_Filepath;
    fs::file_time_type m_LastWriteTime;
};
} // namespace Engine