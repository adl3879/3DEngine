#include "EngineNetAPI.h"

#include <Coral/NativeString.hpp>
#include "Log.h"

namespace Engine
{
static void Log(Coral::NativeString string)
{
    LOG_CORE_INFO("{}", string.ToString());
}

void EngineNetAPI::RegisterMethods()
{
    RegisterMethod("Engine.LoggerLogIcall", reinterpret_cast<void *>(&Log));
}
}
