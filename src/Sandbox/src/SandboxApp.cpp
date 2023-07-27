#include <iostream>

#include "Application.h"
#include "AppLayer.h"
#include "Project.h"

class Sandbox : public Engine::Application
{
  public:
    Sandbox() { PushLayer(new Engine::AppLayer()); }

    ~Sandbox() = default;
};

Engine::Application *Engine::CreateApplication()
{
    // Project::New()->SaveActive("/home/adeleye/Source/3DEngine/src/Sandbox/Untitled.3dproj");
    return new Sandbox();
}
