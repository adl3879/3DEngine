#pragma once

#include "Entity.h"
#include "InputManager.h"

#include "LuaScripting.h"
#include <memory>

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
    LuaScriptableEntity(const std::string &filepath, const std::string &name);
    virtual ~LuaScriptableEntity() = default;

  private:
    std::shared_ptr<LuaScriptInstance> m_Script;
    std::string m_Filepath, m_Name;

  private:
    Entity m_Entity;
    friend class Scene;
};
} // namespace Engine