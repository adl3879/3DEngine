#pragma once

#include "NetAPIModule.h"

namespace Engine
{
class SceneNetAPI : public NetAPIModule
{
  public:
    [[nodiscard]] const std::string GetModuleName() const override { return "Scene"; }

    void RegisterMethods() override;
};
}