#include <iostream>

#include "Application.h"
#include "AppLayer.h"

int main()
{
    std::cout << "Hello World!\n";

    Layer *appLayer = new AppLayer();
    Application::Instance().PushLayer(appLayer);

    Application::Instance().Run();
    return 0;
}
