#pragma once

#include "InputKey.h"

#include <functional>
#include <unordered_map>

enum class InputDeviceType
{
    Keyboard,
    Mouse,
    MouseMove,
    Gamepad
};

struct CursorPosition
{
    double X{0.0f}, Y{0.0f};
    double LastPositionX{0.0f}, LastPositionY{0.0f};
};

struct InputDeviceState
{
    int Value;
};

using InputDeviceStateCallbackFunc = std::function<std::unordered_map<InputKey, InputDeviceState>(int)>;
using CursorPositionCallbackFunc = std::function<CursorPosition(int)>;

struct InputDevice
{
    InputDeviceType Type;
    int Index;
    std::unordered_map<InputKey, InputDeviceState> CurrentState;
    InputDeviceStateCallbackFunc StateFunc;
    CursorPositionCallbackFunc CursorStateFunc;
};
