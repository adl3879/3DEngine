#include "InputManager.h"
#include "InputKey.h"

#include <iostream>
#include <unordered_set>

InputSource GetInputSourceFromKey(InputKey key)
{
    switch (key)
    {
    case InputKey::A:
    case InputKey::B:
    case InputKey::C:
    case InputKey::D:
    case InputKey::E:
    case InputKey::F:
    case InputKey::G:
    case InputKey::H:
    case InputKey::I:
    case InputKey::J:
    case InputKey::K:
    case InputKey::L:
    case InputKey::M:
    case InputKey::N:
    case InputKey::O:
    case InputKey::P:
    case InputKey::Q:
    case InputKey::R:
    case InputKey::S:
    case InputKey::T:
    case InputKey::U:
    case InputKey::V:
    case InputKey::W:
    case InputKey::X:
    case InputKey::Y:
    case InputKey::Z:

    case InputKey::Space:
    case InputKey::Escape:
    case InputKey::Enter:
    case InputKey::Left:
    case InputKey::Right:
    case InputKey::Up:
    case InputKey::Down:
    case InputKey::LeftShift:
    case InputKey::RightShift:
    case InputKey::LeftControl:
    case InputKey::RightControl:

    case InputKey::LeftAlt:
    case InputKey::RightAlt:
    case InputKey::LeftSuper:
    case InputKey::RightSuper:
    case InputKey::Tab:
    case InputKey::Backspace:
    case InputKey::Insert:
    case InputKey::Delete:
    case InputKey::PageUp:
    case InputKey::PageDown:
    case InputKey::Home:
    case InputKey::End:
        return InputSource::Keyboard;

    case InputKey::MOUSE_MOVE_X:
    case InputKey::MOUSE_MOVE_Y:
    case InputKey::MOUSE_MOVE_Z:
    case InputKey::MOUSE_BUTTON_LEFT:
    case InputKey::MOUSE_BUTTON_RIGHT:
    case InputKey::MOUSE_BUTTON_MIDDLE:
        return InputSource::Mouse;

    default:
        return InputSource::Unknown;
    }
}

InputManager::InputManager()
{
    m_Action = true;
    std::cout << "InputManager Initialized" << std::endl;
}

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

void InputManager::ProcessInput()
{
    std::vector<ActionEvent> events{};
    for (auto &device : m_Devices)
    {
        auto newState = device.StateFunc(device.Index);
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
        }
    }

    // propagate action events
    for (auto &event : events)
    {
        PropagateActionEvent(event);
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
    InputSource source = GetInputSourceFromKey(key);

    for (auto &action : actions)
    {
        events.emplace_back(ActionEvent{
            .ActionName = action.ActionName,
            .Source = source,
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
