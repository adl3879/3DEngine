#pragma once

#include "System.h"

namespace Engine
{
class TransformSystem : public System
{
  public:
    TransformSystem(Scene *scene);

    void Update(float dt) override;
};
} // namespace Engine
