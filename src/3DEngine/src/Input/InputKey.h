#pragma once

#include <string>

namespace Engine
{
enum class InputKey
{
    Unknown,

    A,
    B,
    C,
    D,
    E,
    F,
    G,
    H,
    I,
    J,
    K,
    L,
    M,
    N,
    O,
    P,
    Q,
    R,
    S,
    T,
    U,
    V,
    W,
    X,
    Y,
    Z,
    Space,
    Escape,
    Enter,
    Left,
    Right,
    Up,
    Down,
    LeftShift,
    RightShift,
    LeftControl,
    RightControl,
    LeftAlt,
    RightAlt,
    LeftSuper,
    RightSuper,
    Tab,
    Backspace,
    Insert,
    Delete,
    PageUp,
    PageDown,
    Home,
    End,

    MOUSE_POS_X,
    MOUSE_POS_Y,
    MOUSE_MOVE_X,
    MOUSE_MOVE_Y,
    MOUSE_MOVE_Z,
    MOUSE_BUTTON_LEFT,
    MOUSE_BUTTON_RIGHT,
    MOUSE_BUTTON_MIDDLE,
};

enum class InputSource
{
    Keyboard,
    Mouse,
    Unknown
};

struct InputAction
{
    std::string ActionName{""};
    float Scale{1.0f};
};

InputSource GetInputSourceFromKey(InputKey key);
}