#pragma once

#include "InputDevice.h"
#include "InputKey.h"
#include <unordered_map>

namespace Engine
{
class Input
{
  public:
    Input() = default;

    std::unordered_map<InputKey, InputDeviceState> GetKeyboardState(int index) { return m_KeyboardState; }
    std::unordered_map<MouseButton, InputDeviceState> GetMousePressState(int index) { return m_MouseButtonState; }
    MouseMovedPosition GetCursorPosition(int index) { return m_CursorPosition; }
    WindowState GetWindowState(int index) { return m_WindowState; }
    MouseScrollState GetMouseScrollState(int index) { return m_MouseScrollState; }

    void UpdateKeyboardState(int key, float value);
    void UpdateMousePressState(int key, float value);
    void UpdateCursorPosition(double xPos, double yPos);
    void UpdateMouseScrollState(double xOffset, double yOffset);
    void UpdateWindowState(WindowState state);

  private:
    static InputKey KeyToInputKey(int key);
    static MouseButton ButtonToMouseButton(int key);

  private:
    std::unordered_map<InputKey, InputDeviceState> m_KeyboardState;
    std::unordered_map<MouseButton, InputDeviceState> m_MouseButtonState;
    WindowState m_WindowState{};
    MouseMovedPosition m_CursorPosition{};
    MouseScrollState m_MouseScrollState{};
};
}