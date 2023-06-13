#pragma once

#include <entt.hpp>

#include "Scene.h"

namespace Engine
{
class Entity
{
  public:
    Entity() = default;
    Entity(entt::entity handle, Scene *scene);
    Entity(const Entity &other) = default;

    template <typename T, typename... Args> T &AddComponent(Args &&...args)
    {
        if (HasComponent<T>())
            throw std::runtime_error("Entity already has component!");

        return m_Scene->m_Registry.emplace<T>(m_EntityHandle, std::forward<Args>(args)...);
    }

    template <typename T> T &GetComponent()
    {
        if (!HasComponent<T>())
            throw std::runtime_error("Entity does not have component!");

        return m_Scene->m_Registry.get<T>(m_EntityHandle);
    }

    template <typename T> bool HasComponent()
    {
        //
        return m_Scene->m_Registry.any_of<T>(m_EntityHandle);
    }

    template <typename T> void RemoveComponent()
    {
        if (!HasComponent<T>())
            throw std::runtime_error("Entity does not have component!");

        m_Scene->m_Registry.remove<T>(m_EntityHandle);
    }

  private:
    entt::entity m_EntityHandle{0};
    Scene *m_Scene = nullptr;
};

} // namespace Engine