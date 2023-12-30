#pragma once

#include <set>
#include <string>

namespace Engine
{
enum class InputKey
{
    // From glfw3.h
    Space = 32,
    Apostrophe = 39,
    Comma = 44,
    Minus = 45,
    Period = 46,
    Slash = 47,

    D0 = 48,
    D1 = 49,
    D2 = 50,
    D3 = 51,
    D4 = 52,
    D5 = 53,
    D6 = 54,
    D7 = 55,
    D8 = 56,
    D9 = 57,

    Semicolon = 59,
    Equal = 61,

    A = 65,
    B = 66,
    C = 67,
    D = 68,
    E = 69,
    F = 70,
    G = 71,
    H = 72,
    I = 73,
    J = 74,
    K = 75,
    L = 76,
    M = 77,
    N = 78,
    O = 79,
    P = 80,
    Q = 81,
    R = 82,
    S = 83,
    T = 84,
    U = 85,
    V = 86,
    W = 87,
    X = 88,
    Y = 89,
    Z = 90,

    LeftBracket = 91,
    Backslash = 92,
    RightBracket = 93,
    GraveAccent = 96,

    World1 = 161,
    World2 = 162,

    /* Function keys */
    Escape = 256,
    Enter = 257,
    Tab = 258,
    Backspace = 259,
    Insert = 260,
    Delete = 261,
    Right = 262,
    Left = 263,
    Down = 264,
    Up = 265,
    PageUp = 266,
    PageDown = 267,
    Home = 268,
    End = 269,
    CapsLock = 280,
    ScrollLock = 281,
    NumLock = 282,
    PrintScreen = 283,
    Pause = 284,
    F1 = 290,
    F2 = 291,
    F3 = 292,
    F4 = 293,
    F5 = 294,
    F6 = 295,
    F7 = 296,
    F8 = 297,
    F9 = 298,
    F10 = 299,
    F11 = 300,
    F12 = 301,
    F13 = 302,
    F14 = 303,
    F15 = 304,
    F16 = 305,
    F17 = 306,
    F18 = 307,
    F19 = 308,
    F20 = 309,
    F21 = 310,
    F22 = 311,
    F23 = 312,
    F24 = 313,
    F25 = 314,

    /* Keypad */
    KP0 = 320,
    KP1 = 321,
    KP2 = 322,
    KP3 = 323,
    KP4 = 324,
    KP5 = 325,
    KP6 = 326,
    KP7 = 327,
    KP8 = 328,
    KP9 = 329,
    KPDecimal = 330,
    KPDivide = 331,
    KPMultiply = 332,
    KPSubtract = 333,
    KPAdd = 334,
    KPEnter = 335,
    KPEqual = 336,

    LeftShift = 340,
    LeftControl = 341,
    LeftAlt = 342,
    LeftSuper = 343,
    RightShift = 344,
    RightControl = 345,
    RightAlt = 346,
    RightSuper = 347,
    Menu = 348,

    Unknown,
    None,
};

enum class SpecialKey
{
    Shift,
    Control,
    Alt,
    Super,

	ShiftControl,
	ShiftAlt,
	ShiftSuper,
	ControlAlt,
	ControlSuper,
	AltSuper,
	ShiftControlAlt,
	ShiftControlSuper,
	ShiftAltSuper,
	ControlAltSuper,
	ShiftControlAltSuper,

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

namespace Utils
{
static InputKey InputKeyFromString(std::string key)
{
    if (key == "Space") return InputKey::Space;
    if (key == "Apostrophe") return InputKey::Apostrophe;
    if (key == "Comma") return InputKey::Comma;
    if (key == "Minus") return InputKey::Minus;
    if (key == "Period") return InputKey::Period;
    if (key == "Slash") return InputKey::Slash;
    if (key == "D0") return InputKey::D0;
    if (key == "D1") return InputKey::D1;
    if (key == "D2") return InputKey::D2;
    if (key == "D3") return InputKey::D3;
    if (key == "D4") return InputKey::D4;
    if (key == "D5") return InputKey::D5;
    if (key == "D6") return InputKey::D6;
    if (key == "D7") return InputKey::D7;
    if (key == "D8") return InputKey::D8;
    if (key == "D9") return InputKey::D9;
    if (key == "Semicolon") return InputKey::Semicolon;
    if (key == "Equal") return InputKey::Equal;
    if (key == "A") return InputKey::A;
    if (key == "B") return InputKey::B;
    if (key == "C") return InputKey::C;
    if (key == "D") return InputKey::D;
    if (key == "E") return InputKey::E;
    if (key == "F") return InputKey::F;
    if (key == "G") return InputKey::G;
    if (key == "H") return InputKey::H;
    if (key == "I") return InputKey::I;
    if (key == "J") return InputKey::J;
    if (key == "K") return InputKey::K;
    if (key == "L") return InputKey::L;
    if (key == "M") return InputKey::M;
    if (key == "N") return InputKey::N;
    if (key == "O") return InputKey::O;
    if (key == "P") return InputKey::P;
    if (key == "Q") return InputKey::Q;
    if (key == "R") return InputKey::R;
    if (key == "S") return InputKey::S;
    if (key == "T") return InputKey::T;
    if (key == "U") return InputKey::U;
    if (key == "V") return InputKey::V;
    if (key == "W") return InputKey::W;
    if (key == "X") return InputKey::X;
    if (key == "Y") return InputKey::Y;
    if (key == "Z") return InputKey::Z;
    if (key == "LeftBracket") return InputKey::LeftBracket;
    if (key == "Backslash") return InputKey::Backslash;
    if (key == "RightBracket") return InputKey::RightBracket;
    if (key == "GraveAccent") return InputKey::GraveAccent;
    if (key == "World1") return InputKey::World1;
    if (key == "World2") return InputKey::World2;
    if (key == "Escape") return InputKey::Escape;
    if (key == "Enter") return InputKey::Enter;
    if (key == "Tab") return InputKey::Tab;
    if (key == "Backspace") return InputKey::Backspace;
    if (key == "Insert") return InputKey::Insert;
    if (key == "Delete") return InputKey::Delete;
    if (key == "Right") return InputKey::Right;
    if (key == "Left") return InputKey::Left;
    if (key == "Down") return InputKey::Down;
    if (key == "Up") return InputKey::Up;
    if (key == "PageUp") return InputKey::PageUp;
    if (key == "PageDown") return InputKey::PageDown;
    if (key == "Home") return InputKey::Home;
    if (key == "End") return InputKey::End;
    if (key == "CapsLock") return InputKey::CapsLock;
    if (key == "ScrollLock") return InputKey::ScrollLock;
    if (key == "NumLock") return InputKey::NumLock;
    if (key == "PrintScreen") return InputKey::PrintScreen;
    if (key == "Pause") return InputKey::Pause;
    if (key == "F1") return InputKey::F1;
    if (key == "F2") return InputKey::F2;
    if (key == "F3") return InputKey::F3;
    if (key == "F4") return InputKey::F4;
    if (key == "F5") return InputKey::F5;
    if (key == "F6") return InputKey::F6;
    if (key == "F7") return InputKey::F7;
    if (key == "F8") return InputKey::F8;
    if (key == "F9") return InputKey::F9;
    if (key == "F10") return InputKey::F10;
    if (key == "F11") return InputKey::F11;
    if (key == "F12") return InputKey::F12;
    if (key == "F13") return InputKey::F13;
    if (key == "F14") return InputKey::F14;
    if (key == "F15") return InputKey::F15;
    if (key == "F16") return InputKey::F16;
    if (key == "F17") return InputKey::F17;
    if (key == "F18") return InputKey::F18;
    if (key == "F19") return InputKey::F19;
    if (key == "F20") return InputKey::F20;
    if (key == "F21") return InputKey::F21;
    if (key == "F22") return InputKey::F22;
    if (key == "F23") return InputKey::F23;
    if (key == "F24") return InputKey::F24;
    if (key == "F25") return InputKey::F25;
    if (key == "KP0") return InputKey::KP0;
    if (key == "KP1") return InputKey::KP1;
    if (key == "KP2") return InputKey::KP2;
    if (key == "KP3") return InputKey::KP3;
    if (key == "KP4") return InputKey::KP4;
    if (key == "KP5") return InputKey::KP5;
    if (key == "KP6") return InputKey::KP6;
    if (key == "KP7") return InputKey::KP7;
    if (key == "KP8") return InputKey::KP8;
    if (key == "KP9") return InputKey::KP9;
    if (key == "KPDecimal") return InputKey::KPDecimal;
    if (key == "KPDivide") return InputKey::KPDivide;
    if (key == "KPMultiply") return InputKey::KPMultiply;
    if (key == "KPSubtract") return InputKey::KPSubtract;
    if (key == "KPAdd") return InputKey::KPAdd;
    if (key == "KPEnter") return InputKey::KPEnter;
    if (key == "KPEqual") return InputKey::KPEqual;
    if (key == "LeftShift") return InputKey::LeftShift;
    if (key == "LeftControl") return InputKey::LeftControl;
    if (key == "LeftAlt") return InputKey::LeftAlt;
    if (key == "LeftSuper") return InputKey::LeftSuper;
    if (key == "RightShift") return InputKey::RightShift;
    if (key == "RightControl") return InputKey::RightControl;
    if (key == "RightAlt") return InputKey::RightAlt;
    if (key == "RightSuper") return InputKey::RightSuper;
    if (key == "Menu") return InputKey::Menu;
    return InputKey::Unknown;
}

static std::string InputKeyToString(InputKey key)
{
    switch (key)
    {
        case InputKey::Space: return "Space";
        case InputKey::Apostrophe: return "Apostrophe";
        case InputKey::Comma: return "Comma";
        case InputKey::Minus: return "Minus";
        case InputKey::Period: return "Period";
        case InputKey::Slash: return "Slash";
        case InputKey::D0: return "D0";
        case InputKey::D1: return "D1";
        case InputKey::D2: return "D2";
        case InputKey::D3: return "D3";
        case InputKey::D4: return "D4";
        case InputKey::D5: return "D5";
        case InputKey::D6: return "D6";
        case InputKey::D7: return "D7";
        case InputKey::D8: return "D8";
        case InputKey::D9: return "D9";
        case InputKey::Semicolon: return "Semicolon";
        case InputKey::Equal: return "Equal";
        case InputKey::A: return "A";
        case InputKey::B: return "B";
        case InputKey::C: return "C";
        case InputKey::D: return "D";
        case InputKey::E: return "E";
        case InputKey::F: return "F";
        case InputKey::G: return "G";
        case InputKey::H: return "H";
        case InputKey::I: return "I";
        case InputKey::J: return "J";
        case InputKey::K: return "K";
        case InputKey::L: return "L";
        case InputKey::M: return "M";
        case InputKey::N: return "N";
        case InputKey::O: return "O";
        case InputKey::P: return "P";
        case InputKey::Q: return "Q";
        case InputKey::R: return "R";
        case InputKey::S: return "S";
        case InputKey::T: return "T";
        case InputKey::U: return "U";
        case InputKey::V: return "V";
        case InputKey::W: return "W";
        case InputKey::X: return "X";
        case InputKey::Y: return "Y";
        case InputKey::Z: return "Z";
        case InputKey::LeftBracket: return "LeftBracket";
        case InputKey::Backslash: return "Backslash";
        case InputKey::RightBracket: return "RightBracket";
        case InputKey::GraveAccent: return "GraveAccent";
        case InputKey::World1: return "World1";
        case InputKey::World2: return "World2";
        case InputKey::Escape: return "Escape";
        case InputKey::Enter: return "Enter";
        case InputKey::Tab: return "Tab";
        case InputKey::Backspace: return "Backspace";
        case InputKey::Insert: return "Insert";
        case InputKey::Delete: return "Delete";
        case InputKey::Right: return "Right";
        case InputKey::Left: return "Left";
        case InputKey::Down: return "Down";
        case InputKey::Up: return "Up";
        case InputKey::PageUp: return "PageUp";
        case InputKey::PageDown: return "PageDown";
        case InputKey::Home: return "Home";
        case InputKey::End: return "End";
        case InputKey::CapsLock: return "CapsLock";
        case InputKey::ScrollLock: return "ScrollLock";
        case InputKey::NumLock: return "NumLock";
        case InputKey::PrintScreen: return "PrintScreen";
        case InputKey::Pause: return "Pause";
        case InputKey::F1: return "F1";
        case InputKey::F2: return "F2";
        case InputKey::F3: return "F3";
        case InputKey::F4: return "F4";
        case InputKey::F5: return "F5";
        case InputKey::F6: return "F6";
        case InputKey::F7: return "F7";
        case InputKey::F8: return "F8";
        case InputKey::F9: return "F9";
        case InputKey::F10: return "F10";
        case InputKey::F11: return "F11";
        case InputKey::F12: return "F12";
        case InputKey::F13: return "F13";
        case InputKey::F14: return "F14";
        case InputKey::F15: return "F15";
        case InputKey::F16: return "F16";
        case InputKey::F17: return "F17";
        case InputKey::F18: return "F18";
        case InputKey::F19: return "F19";
        case InputKey::F20: return "F20";
        case InputKey::F21: return "F21";
        case InputKey::F22: return "F22";
        case InputKey::F23: return "F23";
        case InputKey::F24: return "F24";
        case InputKey::F25: return "F25";
        case InputKey::KP0: return "KP0";
        case InputKey::KP1: return "KP1";
        case InputKey::KP2: return "KP2";
        case InputKey::KP3: return "KP3";
        case InputKey::KP4: return "KP4";
        case InputKey::KP5: return "KP5";
        case InputKey::KP6: return "KP6";
        case InputKey::KP7: return "KP7";
        case InputKey::KP8: return "KP8";
        case InputKey::KP9: return "KP9";
        case InputKey::KPDecimal: return "KPDecimal";
        case InputKey::KPDivide: return "KPDivide";
        case InputKey::KPMultiply: return "KPMultiply";
        case InputKey::KPSubtract: return "KPSubtract";
        case InputKey::KPAdd: return "KPAdd";
        case InputKey::KPEnter: return "KPEnter";
        case InputKey::KPEqual: return "KPEqual";
        case InputKey::LeftShift: return "LeftShift";
        case InputKey::LeftControl: return "LeftControl";
        case InputKey::LeftAlt: return "LeftAlt";
        case InputKey::LeftSuper: return "LeftSuper";
        case InputKey::RightShift: return "RightShift";
        case InputKey::RightControl: return "RightControl";
        case InputKey::RightAlt: return "RightAlt";
        case InputKey::RightSuper: return "RightSuper";
        case InputKey::Menu: return "Menu";
        case InputKey::Unknown: return "Unknown";
        case InputKey::None: return "None";
    }
}

static std::string SpecialKeyToString(SpecialKey key)
{
    if (key == SpecialKey::Shift) return "Shift";
    if (key == SpecialKey::Control) return "Control";
    if (key == SpecialKey::Alt) return "Alt";
    if (key == SpecialKey::Super) return "Super";
	
	if (key == SpecialKey::ShiftControl) return "Shift + Control";
	if (key == SpecialKey::ShiftAlt) return "Shift + Alt";
	if (key == SpecialKey::ShiftSuper) return "Shift + Super";
	if (key == SpecialKey::ControlAlt) return "Control + Alt";
	if (key == SpecialKey::ControlSuper) return "Control + Super";
	if (key == SpecialKey::AltSuper) return "Alt + Super";
	if (key == SpecialKey::ShiftControlAlt) return "Shift + Control + Alt";
	if (key == SpecialKey::ShiftControlSuper) return "Shift + Control + Super";
	if (key == SpecialKey::ShiftAltSuper) return "Shift + Alt + Super";
	if (key == SpecialKey::ControlAltSuper) return "Control + Alt + Super";
	if (key == SpecialKey::ShiftControlAltSuper) return "Shift + Control + Alt + Super";

    return "";
}

static SpecialKey SpecialKeyFromString(std::string key)
{
	if (key == "Shift") return SpecialKey::Shift;
	if (key == "Control") return SpecialKey::Control;
	if (key == "Alt") return SpecialKey::Alt;
	if (key == "Super") return SpecialKey::Super;
	if (key == "Shift + Control") return SpecialKey::ShiftControl;
	if (key == "Shift + Alt") return SpecialKey::ShiftAlt;
	if (key == "Shift + Super") return SpecialKey::ShiftSuper;
	if (key == "Control + Alt") return SpecialKey::ControlAlt;
	if (key == "Control + Super") return SpecialKey::ControlSuper;
	if (key == "Alt + Super") return SpecialKey::AltSuper;
	if (key == "Shift + Control + Alt") return SpecialKey::ShiftControlAlt;
	if (key == "Shift + Control + Super") return SpecialKey::ShiftControlSuper;
	if (key == "Shift + Alt + Super") return SpecialKey::ShiftAltSuper;
	if (key == "Control + Alt + Super") return SpecialKey::ControlAltSuper;
	if (key == "Shift + Control + Alt + Super") return SpecialKey::ShiftControlAltSuper;
	return SpecialKey::None;
}

static MouseButton MouseButtonFromString(std::string button)
{
    if (button == "Left") return MouseButton::Left;
    if (button == "Right") return MouseButton::Right;
    if (button == "Middle") return MouseButton::Middle;
    return MouseButton::Unknown;
}

static std::string MouseButtonToString(MouseButton button)
{

    if (button == MouseButton::Left) return "Left";
    if (button == MouseButton::Right) return "Right";
    if (button == MouseButton::Middle) return "Middle";
    if (button == MouseButton::Unknown) return "Unknown";
    return "None";
}
} // namespace Utils
} // namespace Engine
