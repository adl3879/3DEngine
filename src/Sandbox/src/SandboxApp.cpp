#include <iostream>

#include "Application.h"
#include "AppLayer.h"

class Sandbox : public Application
{
  public:
    Sandbox() { PushLayer(new AppLayer()); }

    ~Sandbox() = default;
};

Application *CreateApplication() { return new Sandbox(); }
