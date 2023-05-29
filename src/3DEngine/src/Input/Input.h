#pragma once

#include "InputDevice.h"
#include "InputKey.h"
#include <unordered_map>

class Input
{
  public:
    Input() = default;

    std::unordered_map<InputKey, InputDeviceState> GetKeyboardState(int index) { return m_KeyboardState; }
    std::unordered_map<InputKey, InputDeviceState> GetMouseState(int index) { return m_MouseState; }
    CursorPosition GetCursorPosition(int index) { return m_CursorPosition; }
    WindowState GetWindowState(int index) { return m_WindowState; }

    void UpdateKeyboardState(int key, float value);
    void UpdateMouseState(int key, float value);
    void UpdateCursorPosition(double xpos, double ypos);
    void UpdateWindowState(WindowState state);

  private:
    static InputKey KeyToInputKey(int key);
    static InputKey ButtonToInputKey(int key);

  private:
    std::unordered_map<InputKey, InputDeviceState> m_KeyboardState;
    std::unordered_map<InputKey, InputDeviceState> m_MouseState;

    WindowState m_WindowState{};
    CursorPosition m_CursorPosition{};
};
