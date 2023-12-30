#pragma once

#include "NetAPIModule.h"

namespace Engine
{
class EngineNetAPI : public NetAPIModule
{
public:
    [[nodiscard]] const std::string GetModuleName() const override { return "Engine"; }

    void RegisterMethods() override;
};
}
