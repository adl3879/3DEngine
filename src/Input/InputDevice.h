#pragma once

#include "InputKey.h"

#include <functional>
#include <unordered_map>

enum class InputDeviceType
{
    Keyboard,
    Mouse,
    Gamepad
};

struct InputDeviceState
{
    int Value;
};

using InputDeviceStateCallbackFunc = std::function<std::unordered_map<InputKey, InputDeviceState>(int)>;

struct InputDevice
{
    InputDeviceType Type;
    int Index;
    std::unordered_map<InputKey, InputDeviceState> CurrentState;
    InputDeviceStateCallbackFunc StateFunc;
};
