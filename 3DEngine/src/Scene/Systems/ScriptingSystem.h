#pragma once

#include "System.h"

namespace Engine
{
class ScriptingSystem : public System
{
  public:
    ScriptingSystem(Scene *scene);

    bool Init() override;
    void Update(float dt) override;
    void Exit() override;
    void FixedUpdate(float dt) override;

    void OnRuntimeStart() override;
    void OnRuntimeStop() override;
    void OnRuntimeUpdate(float dt) override;
};
} // namespace Engine