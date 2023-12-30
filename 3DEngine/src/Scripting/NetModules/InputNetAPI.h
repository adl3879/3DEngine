#pragma once

#include "NetAPIModule.h"

namespace Engine
{
class InputNetAPI : public NetAPIModule
{
  public:
    [[nodiscard]] const std::string GetModuleName() const override { return "Input"; }

    void RegisterMethods() override;
};
} // namespace Engine