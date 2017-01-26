#include "Engine.h"
#include "InputManager.h"
#include "interfaces/Input.h"
#include "WindowManager.h"

#include <iostream>

#include <GLFW\glfw3.h>

Engine::Engine() 
{
    _inputManager = new InputManager();
    Input::init(_inputManager);

    _windowManager = new WindowManager(_inputManager);
}

Engine::~Engine()
{
    delete _windowManager;
    delete _inputManager;
}

void Engine::run() 
{
    while (!Input::checkKey(GLFW_KEY_ESCAPE)) 
    {
        _inputManager->pollEvents();

        _windowManager->show();
    }
}

int main(int argc, char *argv[])
{

    Engine *engine = new Engine();

    engine->run();

    delete engine;

    return 0;
}