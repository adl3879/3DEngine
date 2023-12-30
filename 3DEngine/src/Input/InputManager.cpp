#include "InputManager.h"
#include "InputDevice.h"
#include "InputKey.h"

#include <unordered_set>
#include <yaml-cpp/yaml.h>

#include "Log.h"

namespace Engine
{
InputManager::InputManager() { m_Action = true; }

InputManager::~InputManager() { m_Action = false; }

InputManager &InputManager::Get()
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

void InputManager::RegisterKeyReleasedCallback(KeyReleasedCallbackFunc callback)
{
    m_KeyReleasedCallbacks.emplace_back(callback);
}

void InputManager::RegisterMousePressedCallback(MousePressCallbackFunc callback)
{
    m_MousePressedCallbacks.emplace_back(callback);
}

void InputManager::RegisterMouseReleasedCallback(MouseReleasedCallbackFunc callback)
{
    m_MouseReleasedCallbacks.emplace_back(callback);
}

void InputManager::RegisterMouseMovedCallback(MouseMovedCallbackFunc callback)
{
    m_MouseMovedCallbacks.emplace_back(callback);
}

void InputManager::RegisterWindowResizeCallback(WindowResizeCallbackFunc callback)
{
    m_WindowResizeCallbacks.emplace_back(callback);
}

void InputManager::RegisterMouseScrollCallback(MouseScrollCallbackFunc callback)
{
    m_MouseScrollCallbacks.emplace_back(callback);
}

void InputManager::MapInputToAction(std::string actionName, InputToActionMapVariant input)
{
    if (actionName == "") return; 
    m_InputToActionMap[actionName].emplace(input);
}

void InputManager::UnmapInputFromAction(std::string actionName, InputToActionMapVariant input)
{
	if (actionName == "") return; 
	std::erase_if(m_InputToActionMap[actionName], [input](const InputToActionMapVariant &inputMap)
					  { return inputMap == input; });
}

void InputManager::UpdateInputToActionMap(std::string actionName, InputToActionMapVariant oldInput, InputToActionMapVariant newInput)
{
	if (actionName == "") return; 
	std::erase_if(m_InputToActionMap[actionName], [oldInput](const InputToActionMapVariant &inputMap)
						  { return inputMap == oldInput; });
	m_InputToActionMap[actionName].emplace(newInput);
}

void InputManager::RemoveAction(std::string actionName)
{ 
	m_InputToActionMap.erase(actionName);
}

void InputManager::ProcessInput()
{
    std::vector<ActionEvent> events{};
    struct MouseMovedCallbackParams
    {
        MouseMovedPosition Position;
        MouseMovedPosition Offset;
        bool IsMoved = false;
    };
    MouseMovedCallbackParams mouseMovedCallbackParams;
    struct WindowCallbackParams
    {
        WindowState State = WindowState{.EventState = WindowEventState::None};
        bool IsResized = false;
    };
    WindowCallbackParams windowCallbackParams;
    struct KeyboardCallbackParams
    {
        InputKey Key = InputKey::None;
        float IsRepeat;
        bool IsPressed = false;
    };
    KeyboardCallbackParams keyCallbackParams{};
    struct MousePressCallbackParams
    {
        MouseButton Button = MouseButton::None;
    };
    MousePressCallbackParams mousePressCallbackParams{};
    struct MouseScrollCallbackParams
    {
        MouseScrollState State;
        bool IsScrolled = false;
    };
    MouseScrollCallbackParams mouseScrollCallbackParams{};
    MouseButton currentMouseState{};

    for (auto &device : m_Devices)
    {
        switch (device.Type)
        {
            case InputDeviceType::Mouse:
            {
                auto newState = device.MousePressStateFunc(device.Index);
                for (auto &mouseState : newState)
                {
                    currentMouseState = device.CurrentMouseButtonState;
                    if (mouseState.second.Value > 0.0f)
                    {
                        mousePressCallbackParams = MousePressCallbackParams{.Button = mouseState.first};
                        device.CurrentMouseButtonState = mouseState.first;
                    }
                }
            }
            break;
            case InputDeviceType::MouseScroll:
            {
                auto newState = device.MouseScrollStateFunc(device.Index);
                auto &currentScrollOffset = device.CurrentMouseScrollState;
                mouseScrollCallbackParams = MouseScrollCallbackParams{.State = newState, .IsScrolled = true};
                device.CurrentMouseScrollState = newState;
            }
            break;
            case InputDeviceType::Window:
            {
                // window event callback
                auto newState = device.WindowStateFunc(device.Index);
                if (newState.Width != device.CurrentWindowState.Width ||
                    newState.Height != device.CurrentWindowState.Height)
                {
                    windowCallbackParams = WindowCallbackParams{.State = newState, .IsResized = true};
                    device.CurrentWindowState = newState;
                }
            }
            break;
            case InputDeviceType::MouseMove:
            {
                // cursor position callback
                auto newState = device.CursorStateFunc(device.Index);
                auto currentPosition = device.CurrentCursorPosition;
                if (newState.X != currentPosition.X || newState.Y != currentPosition.Y)
                {
                    mouseMovedCallbackParams =
                        MouseMovedCallbackParams{.Position = newState,
                                                 .Offset = MouseMovedPosition{.X = newState.X - currentPosition.X,
                                                                              .Y = newState.Y - currentPosition.Y},
                                                 .IsMoved = true};
                    device.CurrentCursorPosition = newState;
                }
            }
            break;
            case InputDeviceType::Keyboard:
            {
                auto newState = device.KeyboardStateFunc(device.Index);
                // compare to old state for changes
                for (auto &keyState : newState)
                {
                    if (device.CurrentKeyboardState[keyState.first].Value != keyState.second.Value)
                    {
                        auto generatedEvents = GenerateActionEvent(device.Index, keyState.first, keyState.second.Value);
                        events.insert(events.end(), generatedEvents.begin(), generatedEvents.end());
                        // save new state Value
                        device.CurrentKeyboardState[keyState.first].Value = keyState.second.Value;
                        if (keyState.second.Value == 1.0f)
                            keyCallbackParams = KeyboardCallbackParams{.Key = keyState.first, .IsRepeat = false};
                        else if (keyState.second.Value == 2.0f)
                            keyCallbackParams = KeyboardCallbackParams{.Key = keyState.first, .IsRepeat = true};
                        else
                            device.CurrentKeyboardState[keyState.first].Value = keyState.second.Value;
                    }
                }
            }
            break;
            case InputDeviceType::Gamepad: break;
        }
    }

    // propagate action events
    for (auto &event : events) PropagateActionEvent(event);

    // keyboard callbacks
    for (auto &callback : m_KeyboardCallbacks)
        if (keyCallbackParams.Key != InputKey::None && !keyCallbackParams.IsRepeat)
            callback(keyCallbackParams.Key, keyCallbackParams.IsRepeat);

    for (auto &callback : m_KeyReleasedCallbacks)
        if (keyCallbackParams.Key != InputKey::None) callback(keyCallbackParams.Key);

    // mouse press callbacks
    for (auto &callback : m_MousePressedCallbacks)
        if (mousePressCallbackParams.Button != MouseButton::None) callback(mousePressCallbackParams.Button);

    // mouse release callbacks
    for (auto &callback : m_MouseReleasedCallbacks)
        if (mousePressCallbackParams.Button == MouseButton::None) callback(currentMouseState);

    // mouse move callbacks
    for (auto &callback : m_MouseMovedCallbacks)
        if (mouseMovedCallbackParams.IsMoved)
            callback(mouseMovedCallbackParams.Position.X, mouseMovedCallbackParams.Position.Y,
                     mouseMovedCallbackParams.Offset.X, mouseMovedCallbackParams.Offset.Y);

    // mouse scroll callbacks
    for (auto &callback : m_MouseScrollCallbacks)
        if (mouseScrollCallbackParams.IsScrolled)
            callback(mouseScrollCallbackParams.State.XOffset, mouseScrollCallbackParams.State.YOffset);

    // window resize callbacks
    for (auto &callback : m_WindowResizeCallbacks)
        if (windowCallbackParams.State.EventState != WindowEventState::None || windowCallbackParams.IsResized)
            callback(windowCallbackParams.State.Width, windowCallbackParams.State.Height);
}

void InputManager::PropagateActionEvent(const ActionEvent &event)
{
    for (auto it = m_ActionCallbacks[event.ActionName].rbegin(); it != m_ActionCallbacks[event.ActionName].rend(); ++it)
    {
        if (it->Func(event.Source, event.SourceIndex, event.Value)) break;
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
            if (state.find(key) != state.end()) return state[key].Value > 0.0f;
        }
    }
    return false;
}
bool InputManager::IsSpecialKeyPressed(SpecialKey key)
{
    if (key == SpecialKey::Shift) return IsKeyPressed(InputKey::LeftShift) || IsKeyPressed(InputKey::RightShift);
    if (key == SpecialKey::Control) return IsKeyPressed(InputKey::LeftControl) || IsKeyPressed(InputKey::RightControl);
    if (key == SpecialKey::Alt) return IsKeyPressed(InputKey::LeftAlt) || IsKeyPressed(InputKey::RightAlt);
    if (key == SpecialKey::Super) return IsKeyPressed(InputKey::LeftSuper) || IsKeyPressed(InputKey::RightSuper);
    if (key == SpecialKey::ShiftControl)
		return (IsKeyPressed(InputKey::LeftShift) || IsKeyPressed(InputKey::RightShift)) &&
			   (IsKeyPressed(InputKey::LeftControl) || IsKeyPressed(InputKey::RightControl));
	if (key == SpecialKey::ShiftAlt)
		return (IsKeyPressed(InputKey::LeftShift) || IsKeyPressed(InputKey::RightShift)) &&
			   (IsKeyPressed(InputKey::LeftAlt) || IsKeyPressed(InputKey::RightAlt));
	if (key == SpecialKey::ShiftSuper)
		return (IsKeyPressed(InputKey::LeftShift) || IsKeyPressed(InputKey::RightShift)) &&
			   (IsKeyPressed(InputKey::LeftSuper) || IsKeyPressed(InputKey::RightSuper));
	if (key == SpecialKey::ControlAlt)
		return (IsKeyPressed(InputKey::LeftControl) || IsKeyPressed(InputKey::RightControl)) &&
			   (IsKeyPressed(InputKey::LeftAlt) || IsKeyPressed(InputKey::RightAlt));
	if (key == SpecialKey::ControlSuper)
		return (IsKeyPressed(InputKey::LeftControl) || IsKeyPressed(InputKey::RightControl)) &&
			   (IsKeyPressed(InputKey::LeftSuper) || IsKeyPressed(InputKey::RightSuper));
	if (key == SpecialKey::AltSuper)
		return (IsKeyPressed(InputKey::LeftAlt) || IsKeyPressed(InputKey::RightAlt)) &&
			   (IsKeyPressed(InputKey::LeftSuper) || IsKeyPressed(InputKey::RightSuper));
	if (key == SpecialKey::ShiftControlAlt)
		return (IsKeyPressed(InputKey::LeftShift) || IsKeyPressed(InputKey::RightShift)) &&
			   (IsKeyPressed(InputKey::LeftControl) || IsKeyPressed(InputKey::RightControl)) &&
			   (IsKeyPressed(InputKey::LeftAlt) || IsKeyPressed(InputKey::RightAlt));
	if (key == SpecialKey::ShiftControlSuper)
		return (IsKeyPressed(InputKey::LeftShift) || IsKeyPressed(InputKey::RightShift)) &&
			   (IsKeyPressed(InputKey::LeftControl) || IsKeyPressed(InputKey::RightControl)) &&
			   (IsKeyPressed(InputKey::LeftSuper) || IsKeyPressed(InputKey::RightSuper));
	if (key == SpecialKey::ShiftAltSuper)
		return (IsKeyPressed(InputKey::LeftShift) || IsKeyPressed(InputKey::RightShift)) &&
			   (IsKeyPressed(InputKey::LeftAlt) || IsKeyPressed(InputKey::RightAlt)) &&
			   (IsKeyPressed(InputKey::LeftSuper) || IsKeyPressed(InputKey::RightSuper));
	if (key == SpecialKey::ControlAltSuper)
		return (IsKeyPressed(InputKey::LeftControl) || IsKeyPressed(InputKey::RightControl)) &&
			   (IsKeyPressed(InputKey::LeftAlt) || IsKeyPressed(InputKey::RightAlt)) &&
			   (IsKeyPressed(InputKey::LeftSuper) || IsKeyPressed(InputKey::RightSuper));
	if (key == SpecialKey::ShiftControlAltSuper)
		return (IsKeyPressed(InputKey::LeftShift) || IsKeyPressed(InputKey::RightShift)) &&
			   (IsKeyPressed(InputKey::LeftControl) || IsKeyPressed(InputKey::RightControl)) &&
			   (IsKeyPressed(InputKey::LeftAlt) || IsKeyPressed(InputKey::RightAlt)) &&
			   (IsKeyPressed(InputKey::LeftSuper) || IsKeyPressed(InputKey::RightSuper));
    return false;
}

bool InputManager::IsMouseButtonPressed(MouseButton button)
{
    for (auto &device : m_Devices)
    {
        if (device.Type == InputDeviceType::Mouse)
        {
            auto state = device.MousePressStateFunc(device.Index);
            if (state.find(button) != state.end()) return state[button].Value > 0.0f;
        }
    }
    return false;
}

MouseMovedPosition InputManager::GetMouseMovedPosition()
{
    for (auto &device : m_Devices)
    {
        if (device.Type == InputDeviceType::MouseMove) return device.CursorStateFunc(device.Index);
    }
    return MouseMovedPosition{};
}

WindowState InputManager::GetWindowState()
{
    for (auto &device : m_Devices)
    {
        if (device.Type == InputDeviceType::Window) return device.WindowStateFunc(device.Index);
    }
    return WindowState{};
}

float InputManager::GetAxis(const std::string &axisName)
{
    if (axisName == "Horizontal")
    {
        if (IsKeyPressed(InputKey::D) || IsKeyPressed(InputKey::Right)) return 1.0f;
        if (IsKeyPressed(InputKey::A) || IsKeyPressed(InputKey::Left)) return -1.0f;
    }
    else if (axisName == "Vertical")
    {
        if (IsKeyPressed(InputKey::W) || IsKeyPressed(InputKey::Up)) return 1.0f;
        if (IsKeyPressed(InputKey::S) || IsKeyPressed(InputKey::Down)) return -1.0f;
    }
    else if (axisName == "Mouse X")
    {
        static float lastMousePosX = 0.0f;
        const auto m = GetMouseMovedPosition().X - lastMousePosX;
        lastMousePosX = static_cast<float>(GetMouseMovedPosition().X);
        return static_cast<float>(m);
    }
    else if (axisName == "Mouse Y")
    {
        static float lastMousePosY = 0.0f;
        const auto m = GetMouseMovedPosition().Y - lastMousePosY;
        lastMousePosY = static_cast<float>(GetMouseMovedPosition().Y);
        return static_cast<float>(m);
    }
    return 0.0f;
}

bool InputManager::GetActionPressed(const std::string &actionName)
{
    if (m_InputToActionMap.find(actionName) == m_InputToActionMap.end()) return false;

    bool pressed = false;
    const auto &inputMapSet = m_InputToActionMap[actionName];

    for (const auto &inputMap : inputMapSet)
    {
        if (std::holds_alternative<std::pair<SpecialKey, InputKey>>(inputMap))
        {
            const auto &input = std::get<std::pair<SpecialKey, InputKey>>(inputMap);
            if (input.first == SpecialKey::None) pressed = IsKeyPressed(input.second);
            else pressed = IsKeyPressed(input.second) && IsSpecialKeyPressed(input.first);
        }
        else if (std::holds_alternative<std::pair<SpecialKey, MouseButton>>(inputMap))
        {
            const auto &input = std::get<std::pair<SpecialKey, MouseButton>>(inputMap);
            if (input.first == SpecialKey::None) pressed = IsMouseButtonPressed(input.second);
            else pressed = IsMouseButtonPressed(input.second) && IsSpecialKeyPressed(input.first);
        }

        if (pressed) break;
    }

    return pressed;
}

InputKey InputManager::GetKey()
{
    if (IsKeyPressed(InputKey::A)) return InputKey::A;
    if (IsKeyPressed(InputKey::B)) return InputKey::B;
    if (IsKeyPressed(InputKey::C)) return InputKey::C;
    if (IsKeyPressed(InputKey::D)) return InputKey::D;
    if (IsKeyPressed(InputKey::E)) return InputKey::E;
    if (IsKeyPressed(InputKey::F)) return InputKey::F;
    if (IsKeyPressed(InputKey::G)) return InputKey::G;
    if (IsKeyPressed(InputKey::H)) return InputKey::H;
    if (IsKeyPressed(InputKey::I)) return InputKey::I;
    if (IsKeyPressed(InputKey::J)) return InputKey::J;
    if (IsKeyPressed(InputKey::K)) return InputKey::K;
    if (IsKeyPressed(InputKey::L)) return InputKey::L;
    if (IsKeyPressed(InputKey::M)) return InputKey::M;
    if (IsKeyPressed(InputKey::N)) return InputKey::N;
    if (IsKeyPressed(InputKey::O)) return InputKey::O;
    if (IsKeyPressed(InputKey::P)) return InputKey::P;
    if (IsKeyPressed(InputKey::Q)) return InputKey::Q;
    if (IsKeyPressed(InputKey::R)) return InputKey::R;
    if (IsKeyPressed(InputKey::S)) return InputKey::S;
    if (IsKeyPressed(InputKey::T)) return InputKey::T;
    if (IsKeyPressed(InputKey::U)) return InputKey::U;
    if (IsKeyPressed(InputKey::V)) return InputKey::V;
    if (IsKeyPressed(InputKey::W)) return InputKey::W;
    if (IsKeyPressed(InputKey::X)) return InputKey::X;
    if (IsKeyPressed(InputKey::Y)) return InputKey::Y;
    if (IsKeyPressed(InputKey::Z)) return InputKey::Z;

    // left, right, up, down
    if (IsKeyPressed(InputKey::Left)) return InputKey::Left;
    if (IsKeyPressed(InputKey::Right)) return InputKey::Right;
    if (IsKeyPressed(InputKey::Up)) return InputKey::Up;
    if (IsKeyPressed(InputKey::Down)) return InputKey::Down;

    if (IsKeyPressed(InputKey::Space)) return InputKey::Space;
    if (IsKeyPressed(InputKey::Enter)) return InputKey::Enter;

    return InputKey::None;
}

MouseButton InputManager::GetMouseButton()
{
    if (IsMouseButtonPressed(MouseButton::Left)) return MouseButton::Left;
    if (IsMouseButtonPressed(MouseButton::Right)) return MouseButton::Right;
    if (IsMouseButtonPressed(MouseButton::Middle)) return MouseButton::Middle;

    return MouseButton::None;
}

SpecialKey InputManager::GetSpecialKey()
{
	if (IsSpecialKeyPressed(SpecialKey::Shift)) return SpecialKey::Shift;
	if (IsSpecialKeyPressed(SpecialKey::Control)) return SpecialKey::Control;
	if (IsSpecialKeyPressed(SpecialKey::Alt)) return SpecialKey::Alt;
	if (IsSpecialKeyPressed(SpecialKey::Super)) return SpecialKey::Super;
	if (IsSpecialKeyPressed(SpecialKey::ShiftControl)) return SpecialKey::ShiftControl;
	if (IsSpecialKeyPressed(SpecialKey::ShiftAlt)) return SpecialKey::ShiftAlt;
	if (IsSpecialKeyPressed(SpecialKey::ShiftSuper)) return SpecialKey::ShiftSuper;
	if (IsSpecialKeyPressed(SpecialKey::ControlAlt)) return SpecialKey::ControlAlt;
	if (IsSpecialKeyPressed(SpecialKey::ControlSuper)) return SpecialKey::ControlSuper;
	if (IsSpecialKeyPressed(SpecialKey::AltSuper)) return SpecialKey::AltSuper;
	if (IsSpecialKeyPressed(SpecialKey::ShiftControlAlt)) return SpecialKey::ShiftControlAlt;
	if (IsSpecialKeyPressed(SpecialKey::ShiftControlSuper)) return SpecialKey::ShiftControlSuper;
	if (IsSpecialKeyPressed(SpecialKey::ShiftAltSuper)) return SpecialKey::ShiftAltSuper;
	if (IsSpecialKeyPressed(SpecialKey::ControlAltSuper)) return SpecialKey::ControlAltSuper;
	if (IsSpecialKeyPressed(SpecialKey::ShiftControlAltSuper)) return SpecialKey::ShiftControlAltSuper;
	return SpecialKey::None;
}
} // namespace Engine
