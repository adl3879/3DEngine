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
    void Draw() override;
    void FixedUpdate(float dt) override;
    void Exit() override;

  private:
    void InitializeShapes();
    void InitializeRigidbodies();
    // void InitializeQuakeMap();
    // void InitializeCharacterControllers();

    void ApplyForces();
};
} // namespace Engine