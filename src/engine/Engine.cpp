#include "Engine.h"
#include "InputManager.h"
#include "interfaces/Input.h"
#include "WindowManager.h"

#include <iostream>

Engine::Engine() 
{
    _inputManager = new InputManager();
    Input::init(_inputManager);

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