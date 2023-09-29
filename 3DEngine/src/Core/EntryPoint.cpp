#include "Application.h"

#include <iostream>

extern Engine::Application *CreateApplication();

int main(int argc, char **argv)
{
    auto app = Engine::CreateApplication();
    app->Run();
    delete app;
}
