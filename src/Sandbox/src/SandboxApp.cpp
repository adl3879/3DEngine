#include <iostream>

#include "Application.h"
#include "AppLayer.h"

class Sandbox : public Engine::Application
{
  public:
    Sandbox() { PushLayer(new AppLayer()); }

    ~Sandbox() = default;
};

Engine::Application *Engine::CreateApplication() { return new Sandbox(); }
