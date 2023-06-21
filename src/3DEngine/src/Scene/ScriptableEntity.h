#pragma once

#include "Entity.h"
#include "InputManager.h"

#include "LuaScriptEngine.h"

namespace Engine
{

class ScriptableEntity
{
  public:
    virtual ~ScriptableEntity() = default;

    template <typename T> T &GetComponent() { return m_Entity.GetComponent<T>(); }

  protected:
    virtual void OnCreate() {}
    virtual void OnDestroy() {}
    virtual void OnUpdate(float dt) {}

  private:
    Entity m_Entity;
    friend class Scene;
};

class LuaScriptableEntity
{
  public:
    LuaScriptableEntity() = default;
    LuaScriptableEntity(const std::string &filepath, const std::string &name);
    virtual ~LuaScriptableEntity() = default;

  public:
    void OnCreate();
    void OnDestroy();
    void OnUpdate(float dt);

  private:
    void OnKeyPressed(InputKey key, bool isRepeat) { m_Script.OnKeyPressed(key, isRepeat); }
    void OnKeyReleased(InputKey key) { m_Script.OnKeyReleased(key); }
    void OnMouseButtonPressed(MouseButton button) { m_Script.OnMouseButtonPressed(button); }
    void OnMouseButtonReleased(MouseButton button) { m_Script.OnMouseButtonReleased(button); }
    void OnMouseMoved(double xPos, double yPos, double xOffset, double yOffset)
    {
        m_Script.OnMouseMoved(xPos, yPos, xOffset, yOffset);
    }
    void OnMouseScrolled(double xOffset, double yOffset) { m_Script.OnMouseScrolled(xOffset, yOffset); }

  private:
    std::string m_Filepath, m_Name;
    CallFunctions m_Script;

  private:
    Entity m_Entity;
    friend class Scene;
};
} // namespace Engine