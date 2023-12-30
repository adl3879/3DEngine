#pragma once

#include "System.h"

namespace Engine
{
class PhysicsSystem : public System
{
public:
    PhysicsSystem(Scene *scene);

    bool Init() override;
    void Update(float dt) override;
    void FixedUpdate(float dt) override;
    void Exit() override;

private:
    void InitializeShapes();
    void InitializeRigidbodies();

    void ApplyForces();
};
} // namespace Engine
