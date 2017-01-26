#include "Engine.h"
#include "WindowManager.h"

#include <iostream>

Engine::Engine() 
{
    _windowManager = new WindowManager();
}

Engine::~Engine()
{
    delete _windowManager;
}

int main(int argc, char *argv[])
{

    Engine *engine = new Engine();

    std::cin.ignore();

    delete engine;

    return 0;
}