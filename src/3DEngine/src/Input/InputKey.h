#pragma once

#include <string>

namespace Engine
{
enum class InputKey
{
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

    Unknown,
    None,
};

enum class MouseButton
{
    Left,
    Right,
    Middle,
    Unknown,
    None,
};

enum class MouseMovedState
{
    X,
    Y,
};

enum class InputSource
{
    Keyboard,
    MouseButton,
    MouseMoved,
    Unknown
};

struct InputAction
{
    std::string ActionName{""};
    float Scale{1.0f};
};
} // namespace Engine