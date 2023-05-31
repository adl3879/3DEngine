#include "InputManager.h"
#include "InputDevice.h"
#include "InputKey.h"

#include <iostream>
#include <unordered_set>

namespace Engine
{
InputManager::InputManager() { m_Action = true; }

InputManager::~InputManager() { m_Action = false; }

InputManager &InputManager::Instance()
{
    static InputManager instance;
    return instance;
}

void InputManager::RegisterActionCallback(const std::string &actionName, const ActionCallback &callback)
{
    m_ActionCallbacks[actionName].emplace_back(callback);
}

void InputManager::RemoveActionCallback(const std::string &actionName, const std::string &callbackRef)
{
    std::erase_if(m_ActionCallbacks[actionName],
                  [callbackRef](ActionCallback &callback) { return callback.Ref == callbackRef; });
}

void InputManager::MapInputToAction(InputKey key, const InputAction &action)
{
    // TODO: check for duplicates
    m_InputActionMapping[key].emplace_back(action);
}

void InputManager::UnmapInputFromAction(InputKey key, const std::string &action)
{
    std::erase_if(m_InputActionMapping[key],
                  [action](const InputAction &inputAction) { return inputAction.ActionName == action; });
}

void InputManager::RegisterKeyboardCallback(KeyboardCallbackFunc callback)
{
    m_KeyboardCallbacks.emplace_back(callback);
}

void InputManager::RegisterMousePressedCallback(MousePressCallbackFunc callback)
{
    m_MousePressedCallbacks.emplace_back(callback);
}

void InputManager::RegisterCursorCallback(CursorCallbackFunc callback) { m_CursorCallbacks.emplace_back(callback); }

void InputManager::ProcessInput()
{
    std::vector<ActionEvent> events{};
    CursorPosition cursorState{};
    WindowState windowState{};

    struct KeyboardCallbackParams
    {
        InputKey Key;
        float isRepeat;
    };
    KeyboardCallbackParams keyCallbackParams{};
    struct MousePressCallbackParams
    {
        MouseButton Button;
    };
    MousePressCallbackParams mousePressCallbackParams{};

    for (auto &device : m_Devices)
    {
        if (device.Type == InputDeviceType::MouseMove)
        {
            // cursor position callback
            cursorState = device.CursorStateFunc(device.Index);
        }
        else if (device.Type == InputDeviceType::Window)
        {
            // window event callback
            windowState = device.WindowStateFunc(device.Index);
        }
        else if (device.Type == InputDeviceType::Mouse)
        {
            auto newState = device.MousePressStateFunc(device.Index);
            for (auto &mouseState : newState)
            {
                if (mouseState.second.Value > 0.0f)
                {
                    mousePressCallbackParams = MousePressCallbackParams{.Button = mouseState.first};
                }
            }
        }
        else if (device.Type == InputDeviceType::Keyboard)
        {
            auto newState = device.KeyboardStateFunc(device.Index);
            // compare to old state for changes
            for (auto &keyState : newState)
            {
                if (device.CurrentState[keyState.first].Value != keyState.second.Value)
                {
                    auto generatedEvents = GenerateActionEvent(device.Index, keyState.first, keyState.second.Value);
                    events.insert(events.end(), generatedEvents.begin(), generatedEvents.end());
                    // save new state Value
                    device.CurrentState[keyState.first].Value = keyState.second.Value;
                }
                if (keyState.second.Value > 0.0f)
                {
                    keyCallbackParams = KeyboardCallbackParams{.Key = keyState.first, .isRepeat = false};
                }
            }
        }
        else
        {
            return;
        }
    }

    // propagate action events
    for (auto &event : events)
    {
        PropagateActionEvent(event);
    }

    // keyboard callbacks
    for (auto &callback : m_KeyboardCallbacks)
    {
        callback(keyCallbackParams.Key, keyCallbackParams.isRepeat);
    }

    // mouse press callbacks
    for (auto &callback : m_MousePressedCallbacks)
    {
        callback(mousePressCallbackParams.Button);
    }

    // mouse move callbacks
    for (auto &callback : m_CursorCallbacks)
    {
        callback(cursorState.X, cursorState.Y);
    }
}

void InputManager::PropagateActionEvent(const ActionEvent &event)
{
    for (auto it = m_ActionCallbacks[event.ActionName].rbegin(); it != m_ActionCallbacks[event.ActionName].rend(); ++it)
    {
        if (it->Func(event.Source, event.SourceIndex, event.Value))
            break;
    }
}

std::vector<InputManager::ActionEvent> InputManager::GenerateActionEvent(int deviceIndex, InputKey key, float newVal)
{
    std::vector<ActionEvent> events{};
    auto &actions = m_InputActionMapping[key];

    for (auto &action : actions)
    {
        events.emplace_back(ActionEvent{
            .ActionName = action.ActionName,
            .Source = InputSource::Keyboard,
            .SourceIndex = deviceIndex,
            .Value = newVal * action.Scale,
        });
    }
    return events;
}

void InputManager::RegisterDevice(InputDevice device) { m_Devices.emplace_back(device); }

void InputManager::UnregisterDevice(InputDeviceType type, int inputIndex)
{
    std::erase_if(m_Devices, [type, inputIndex](const InputDevice &device)
                  { return device.Type == type && device.Index == inputIndex; });
}

bool InputManager::IsKeyPressed(InputKey key)
{
    for (auto &device : m_Devices)
    {
        if (device.Type == InputDeviceType::Keyboard)
        {
            auto state = device.KeyboardStateFunc(device.Index);
            if (state.find(key) != state.end())
                return state[key].Value > 0.0f;
        }
    }
    return false;
}

WindowState InputManager::GetWindowState()
{
    for (auto &device : m_Devices)
    {
        if (device.Type == InputDeviceType::Window)
            return device.WindowStateFunc(device.Index);
    }
    return WindowState{};
}
} // namespace Engine