#include "Application.h"

#include <iostream>
#include <PlatformUtils.h>

extern Engine::Application *CreateApplication();

int main(int argc, char **argv)
{
    auto app = Engine::CreateApplication();
    app->Run();
    delete app;

    Engine::Utils::FileDialogs::Reset();
}
