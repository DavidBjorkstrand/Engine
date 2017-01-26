#pragma once

class WindowManager;
class InputManager;

class Engine
{
    public:
        Engine();
        ~Engine();

    private:
        WindowManager *_windowManager;
        InputManager *_inputManager;

};
