#pragma once

#include <string>
#include "InputManager.h"

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

  public:
    sol::state *GetLuaState() { return m_LuaState; }
    const LuaCallbackFunctions &GetCallbackFunctions() const { return m_CallbackFunctions; }

    void RegisterFunction1S(const std::string &name, std::function<void(const std::string &)> function);

  private:
    sol::state *m_LuaState = nullptr;
    LuaCallbackFunctions m_CallbackFunctions;

    friend class LuaScriptableEntity;
};
} // namespace Engine