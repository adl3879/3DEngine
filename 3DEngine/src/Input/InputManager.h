#pragma once

#include "InputKey.h"
#include "InputDevice.h"

#include <functional>
#include <unordered_map>
#include <set>
#include <vector>
#include <string>
#include <variant>

namespace Engine
{
using InputToActionMapVariant = std::variant<std::pair<SpecialKey, InputKey>, std::pair<SpecialKey, MouseButton>>;

class InputManager
{
  public:
    using ActionCallbackFunc = std::function<bool(InputSource, int, float)>;
    using KeyboardCallbackFunc = std::function<void(InputKey key, bool isRepeat)>;
    using KeyReleasedCallbackFunc = std::function<void(InputKey key)>;
    using MousePressCallbackFunc = std::function<void(MouseButton key)>;
    using MouseReleasedCallbackFunc = std::function<void(MouseButton key)>;
    using MouseMovedCallbackFunc = std::function<void(double xPos, double yPos, double xOffset, double yOffset)>;
    using WindowResizeCallbackFunc = std::function<void(int width, int height)>;
    using MouseScrollCallbackFunc = std::function<void(double xOffset, double yOffset)>;

    struct ActionCallback
    {
        std::string Ref;
        ActionCallbackFunc Func;
    };

  private:
    struct ActionEvent
    {
        std::string ActionName;
        InputSource Source;
        int SourceIndex;
        float Value;
    };

  public:
    virtual ~InputManager();

  private:
    InputManager();

    friend class Application;

    void ProcessInput();
    std::vector<ActionEvent> GenerateActionEvent(int deviceIndex, InputKey key, float newVal);
    void PropagateActionEvent(const ActionEvent &event);
    // void PropagateKeyboardEvent(InputKey key, float isRepeat = false);

  public:
    static InputManager &Get();

    void RegisterActionCallback(const std::string &actionName, const ActionCallback &callback);
    void RemoveActionCallback(const std::string &actionName, const std::string &callbackRef);

    void MapInputToAction(InputKey key, const InputAction &action);
    void UnmapInputFromAction(InputKey key, const std::string &action);

    void RegisterDevice(InputDevice device);
    void UnregisterDevice(InputDeviceType source, int inputIndex);

    // register shortcuts
    void RegisterKeyboardShortcut(std::vector<InputKey> keys, std::function<void()> callback);

    // event callbacks
    void RegisterKeyboardCallback(KeyboardCallbackFunc callback);
    void RegisterKeyReleasedCallback(KeyReleasedCallbackFunc callback);
    void RegisterMousePressedCallback(MousePressCallbackFunc callback);
    void RegisterMouseReleasedCallback(MouseReleasedCallbackFunc callback);
    void RegisterMouseMovedCallback(MouseMovedCallbackFunc callback);
    void RegisterWindowResizeCallback(WindowResizeCallbackFunc callback);
    void RegisterMouseScrollCallback(MouseScrollCallbackFunc callback);

    //
    void MapInputToAction(std::string actionName, InputToActionMapVariant input);
    void UnmapInputFromAction(std::string actionName, InputToActionMapVariant input);
	void UpdateInputToActionMap(std::string actionName, InputToActionMapVariant oldInput, InputToActionMapVariant newInput);
    void RemoveAction(std::string actionName);
    [[nodiscard]] auto GetInputToActionMap() const { return m_InputToActionMap; }
    void SerializeInputToActionMap();

    bool IsKeyPressed(InputKey key);
    bool IsSpecialKeyPressed(SpecialKey key);
    bool IsMouseButtonPressed(MouseButton button);
    MouseMovedPosition GetMouseMovedPosition();
    WindowState GetWindowState();
    float GetAxis(const std::string &axisName);
    bool GetActionPressed(const std::string &actionName);
    InputKey GetKey();
	MouseButton GetMouseButton();
	SpecialKey GetSpecialKey();

  private:
    bool m_Action{false};

    std::unordered_map<InputKey, std::vector<InputAction>> m_InputActionMapping{};
    std::unordered_map<std::string, std::vector<ActionCallback>> m_ActionCallbacks{};

    std::unordered_map<std::string, std::set<InputToActionMapVariant>> m_InputToActionMap{};

    std::vector<KeyboardCallbackFunc> m_KeyboardCallbacks{};
    std::vector<KeyReleasedCallbackFunc> m_KeyReleasedCallbacks{};
    std::vector<MousePressCallbackFunc> m_MousePressedCallbacks{};
    std::vector<MouseReleasedCallbackFunc> m_MouseReleasedCallbacks{};
    std::vector<MouseMovedCallbackFunc> m_MouseMovedCallbacks{};
    std::vector<WindowResizeCallbackFunc> m_WindowResizeCallbacks{};
    std::vector<MouseScrollCallbackFunc> m_MouseScrollCallbacks{};

    std::vector<InputDevice> m_Devices{};

    double m_LastMousePosX{0.0f};
    double m_LastMousePosY{0.0f};
};
} // namespace Engine
