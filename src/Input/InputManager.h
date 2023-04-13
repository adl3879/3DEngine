#pragma once

#include "InputKey.h"
#include "InputDevice.h"

#include <functional>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <string>

class InputManager
{
  public:
    using ActionCallbackFunc = std::function<bool(InputSource, int, float)>;

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

  private:
    InputManager();
    ~InputManager();

  private:
    friend class Application;

    void ProcessInput();
    std::vector<ActionEvent> GenerateActionEvent(int deviceIndex, InputKey key, float newVal);
    void PropagateActionEvent(const ActionEvent &event);

  public:
    static InputManager &Instance();

    void RegisterActionCallback(const std::string &actionName, const ActionCallback &callback);
    void RemoveActionCallback(const std::string &actionName, const std::string &callbackRef);

    void MapInputToAction(InputKey key, const InputAction &action);
    void UnmapInputFromAction(InputKey key, const std::string &action);

    void RegisterDevice(InputDevice device);
    void UnregisterDevice(InputDeviceType source, int inputIndex);

  private:
    bool m_Action{false};

    std::unordered_map<InputKey, std::vector<InputAction>> m_InputActionMapping{};
    std::unordered_map<std::string, std::vector<ActionCallback>> m_ActionCallbacks{};

    std::vector<InputDevice> m_Devices{};
};
