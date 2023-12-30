#include "InputNetAPI.h"

#include "InputManager.h"

#include <Coral/NativeArray.h>
#include <Coral/NativeString.hpp>

namespace Engine
{
static bool IsKeyDown(int keyCode) { return InputManager::Get().IsKeyPressed(static_cast<InputKey>(keyCode)); }

static Coral::NativeArray<float> GetMousePosition()
{
    const auto mousePos = InputManager::Get().GetMouseMovedPosition();
    return Coral::NativeArray<float>({static_cast<float>(mousePos.X), static_cast<float>(mousePos.Y)});
}

static bool IsMouseButtonDown(int mouseButton)
{
    return InputManager::Get().IsMouseButtonPressed(static_cast<MouseButton>(mouseButton));
}

static Coral::NativeArray<float> GetMouseDelta()
{
    static float lastMousePosX = 0.0f;
    static float lastMousePosY = 0.0f;
    const auto mousePos = InputManager::Get().GetMouseMovedPosition();
    const auto mouseDeltaX = static_cast<float>(mousePos.X) - lastMousePosX;
    const auto mouseDeltaY = static_cast<float>(mousePos.Y) - lastMousePosY;

    lastMousePosX = static_cast<float>(mousePos.X);
    lastMousePosY = static_cast<float>(mousePos.Y);
    return {mouseDeltaX, mouseDeltaY};
}

static float GetAxis(Coral::NativeString axisName) { return InputManager::Get().GetAxis(axisName.ToString()); }

static bool IsActionPressed(Coral::NativeString actionName)
{
    return InputManager::Get().GetActionPressed(actionName.ToString());
}

void InputNetAPI::RegisterMethods()
{
    RegisterMethod("Input.IsKeyDownICall", reinterpret_cast<void *>(&IsKeyDown));
    RegisterMethod("Input.GetMousePositionICall", reinterpret_cast<void *>(&GetMousePosition));
    RegisterMethod("Input.IsMouseButtonDownICall", reinterpret_cast<void *>(&IsMouseButtonDown));
    RegisterMethod("Input.GetMouseDeltaICall", reinterpret_cast<void *>(&GetMouseDelta));
    RegisterMethod("Input.GetAxisICall", reinterpret_cast<void *>(&GetAxis));
    RegisterMethod("Input.IsActionPressedICall", reinterpret_cast<void *>(&IsActionPressed));
}
} // namespace Engine
