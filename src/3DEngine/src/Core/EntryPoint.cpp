#include "Application.h"

#include <iostream>

extern Engine::Application *CreateApplication();

int main(int argc, char **argv)
{
    // std::string cmd = "a = 7 + 11";
    // lua_State *L = luaL_newstate();
    // luaL_openlibs(L);

    // int r = luaL_dofile(L, "/home/adeleye/Source/3DEngine/src/Sandbox/res/scripts/main.lua");
    // if (r != LUA_OK)
    // {
    //     std::cout << "Error: " << lua_tostring(L, -1) << std::endl;
    // }
    // else
    // {
    //     lua_getglobal(L, "a");
    //     std::cout << "a = " << (float)lua_tonumber(L, -1) << std::endl;
    // }
    // lua_close(L);

    auto app = Engine::CreateApplication();
    app->Run();
    delete app;
}
