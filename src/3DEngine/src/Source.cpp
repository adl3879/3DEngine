#include "Application.h"

extern Application *CreateApplication();

int main(int argc, char **argv)
{
    auto app = CreateApplication();
    app->Run();
    delete app;
}
