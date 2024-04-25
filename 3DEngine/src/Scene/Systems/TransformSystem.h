#pragma once

#include "System.h"

namespace Engine
{
class Entity;

class TransformSystem : public System
{
  public:
    TransformSystem(Scene *scene);

	bool Init() override;
    void Update(float dt) override;

  private:
    void UpdateTransform();
    void UpdateDirtyFlagRecursive(Entity &entity);
    void CalculateGlobalTransform(Entity &entity);
};
} // namespace Engine
