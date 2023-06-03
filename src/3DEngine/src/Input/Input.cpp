#include "Input.h"
#include "GLFW/glfw3.h"
#include "InputKey.h"

namespace Engine
{
void Input::UpdateKeyboardState(int key, float value)
{
    InputKey inputKey = KeyToInputKey(key);
    m_KeyboardState[inputKey].Value = value;
}

void Input::UpdateMousePressState(int key, float value)
{
    MouseButton mouseButton = ButtonToMouseButton(key);
    m_MouseButtonState[mouseButton].Value = value;
}

void Input::UpdateCursorPosition(double xpos, double ypos) { m_CursorPosition = CursorPosition{.X = xpos, .Y = ypos}; }

void Input::UpdateMouseScrollState(double xOffset, double yOffset)
{
    m_MouseScrollState = MouseScrollState{.XOffset = xOffset, .YOffset = yOffset};
}

void Input::UpdateWindowState(WindowState state) { m_WindowState = state; }

InputKey Input::KeyToInputKey(int key)
{
    switch (key)
    {
    case GLFW_KEY_A:
        return InputKey::A;
    case GLFW_KEY_B:
        return InputKey::B;
    case GLFW_KEY_C:
        return InputKey::C;
    case GLFW_KEY_D:
        return InputKey::D;
    case GLFW_KEY_E:
        return InputKey::E;
    case GLFW_KEY_F:
        return InputKey::F;
    case GLFW_KEY_G:
        return InputKey::G;
    case GLFW_KEY_H:
        return InputKey::H;
    case GLFW_KEY_I:
        return InputKey::I;
    case GLFW_KEY_J:
        return InputKey::J;
    case GLFW_KEY_K:
        return InputKey::K;
    case GLFW_KEY_L:
        return InputKey::L;
    case GLFW_KEY_M:
        return InputKey::M;
    case GLFW_KEY_N:
        return InputKey::N;
    case GLFW_KEY_O:
        return InputKey::O;
    case GLFW_KEY_P:
        return InputKey::P;
    case GLFW_KEY_Q:
        return InputKey::Q;
    case GLFW_KEY_R:
        return InputKey::R;
    case GLFW_KEY_S:
        return InputKey::S;
    case GLFW_KEY_T:
        return InputKey::T;
    case GLFW_KEY_U:
        return InputKey::U;
    case GLFW_KEY_V:
        return InputKey::V;
    case GLFW_KEY_W:
        return InputKey::W;
    case GLFW_KEY_X:
        return InputKey::X;
    case GLFW_KEY_Y:
        return InputKey::Y;
    case GLFW_KEY_Z:
        return InputKey::Z;
    case GLFW_KEY_SPACE:
        return InputKey::Space;
    case GLFW_KEY_ESCAPE:
        return InputKey::Escape;
    case GLFW_KEY_ENTER:
        return InputKey::Enter;
    case GLFW_KEY_LEFT:
        return InputKey::Left;
    case GLFW_KEY_RIGHT:
        return InputKey::Right;
    case GLFW_KEY_UP:
        return InputKey::Up;
    case GLFW_KEY_DOWN:
        return InputKey::Down;
    case GLFW_KEY_LEFT_SHIFT:
        return InputKey::LeftShift;
    case GLFW_KEY_RIGHT_SHIFT:
        return InputKey::RightShift;
    case GLFW_KEY_LEFT_CONTROL:
        return InputKey::LeftControl;
    case GLFW_KEY_RIGHT_CONTROL:
        return InputKey::RightControl;
    case GLFW_KEY_LEFT_ALT:
        return InputKey::LeftAlt;
    case GLFW_KEY_RIGHT_ALT:
        return InputKey::RightAlt;
    case GLFW_KEY_LEFT_SUPER:
        return InputKey::LeftSuper;
    case GLFW_KEY_RIGHT_SUPER:
        return InputKey::RightSuper;
    case GLFW_KEY_TAB:
        return InputKey::Tab;
    case GLFW_KEY_BACKSPACE:
        return InputKey::Backspace;
    case GLFW_KEY_INSERT:
        return InputKey::Insert;
    case GLFW_KEY_DELETE:
        return InputKey::Delete;
    case GLFW_KEY_PAGE_UP:
        return InputKey::PageUp;
    case GLFW_KEY_PAGE_DOWN:
        return InputKey::PageDown;
    case GLFW_KEY_HOME:
        return InputKey::Home;
    case GLFW_KEY_END:
        return InputKey::End;
    default:
        return InputKey::Unknown;
    }
}

MouseButton Input::ButtonToMouseButton(int key)
{
    switch (key)
    {
    case GLFW_MOUSE_BUTTON_LEFT:
        return MouseButton::Left;
    case GLFW_MOUSE_BUTTON_RIGHT:
        return MouseButton::Right;
    case GLFW_MOUSE_BUTTON_MIDDLE:
        return MouseButton::Middle;
    default:
        return MouseButton::Unknown;
    }
}
} // namespace Engine