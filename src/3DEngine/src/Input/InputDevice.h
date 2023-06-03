#pragma once

#include "InputKey.h"

#include <functional>
#include <unordered_map>

namespace Engine
{
enum class InputDeviceType
{
    Keyboard,
    Mouse,
    MouseMove,
    MouseScroll,
    Gamepad,
    Window
};

enum class WindowEventState
{
    None = 0,
    Close,
    Resize,
    Focus,
    LostFocus,
    Moved
};

struct CursorPosition
{
    double X{0.0f}, Y{0.0f};
};

struct MouseScrollState
{
    double XOffset{0.0f}, YOffset{0.0f};
};

struct InputDeviceState
{
    int Value;
};

struct WindowState
{
    WindowEventState EventState;
    int Width = 1600;
    int Height = 900;
};

using InputDeviceStateCallbackFunc = std::function<std::unordered_map<InputKey, InputDeviceState>(int)>;
using CursorPositionCallbackFunc = std::function<CursorPosition(int)>;
using WindowStateCallbackFunc = std::function<WindowState(int)>;
using MousePressCallbackFunc = std::function<std::unordered_map<MouseButton, InputDeviceState>(int)>;
using MouseScrollCallbackFunc = std::function<MouseScrollState(int)>;

struct InputDevice
{
    InputDeviceType Type;
    int Index;
    std::unordered_map<InputKey, InputDeviceState> CurrentKeyboardState;
    WindowState CurrentWindowState = WindowState{.EventState = WindowEventState::None};
    CursorPosition CurrentCursorPosition;
    MouseScrollState CurrentMouseScrollState;

    InputDeviceStateCallbackFunc KeyboardStateFunc;
    CursorPositionCallbackFunc CursorStateFunc;
    WindowStateCallbackFunc WindowStateFunc;
    MousePressCallbackFunc MousePressStateFunc;
    MouseScrollCallbackFunc MouseScrollStateFunc;
};
} // namespace Engine
