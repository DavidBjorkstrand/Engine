#include "Engine.h"
#include "renderer\WindowManager.h"

#include <iostream>

Engine::Engine() 
{
    WindowManager::init();
}

Engine::~Engine()
{
    WindowManager::terminate();
}

int main(int argc, char *argv[])
{

    Engine *engine = new Engine();

    std::cin.ignore();

    delete engine;

    return 0;
}