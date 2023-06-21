#pragma once

#include <string>
#include <unordered_map>
#include <any>
#include <InputManager.h>

struct lua_State;

namespace Engine
{
struct CallFunctions
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

class LuaScriptEngine
{
  public:
    static CallFunctions LoadScript(const std::string &filepath, const std::string &name);
    static void UnloadScript(const std::string &name);

    template <class ReturnType, class... Params>
    static void RegisterFunction(const std::string &name, ReturnType (*fp)(Params...))
    {
        s_Functions[name] = fp;
    }

  private:
    static bool CheckLua(int r);

  private:
    static std::unordered_map<std::string, std::any> s_Functions;
};

} // namespace Engine