#pragma once

class WindowManager;
class InputManager;

/* Main class for the engine */
class Engine
{
    public:
        /*
        * Constructor
        */
        Engine();

        /*
        * Deconstructor
        */
        ~Engine();

        /*
        * Starts the game loop
        */
        void run();

    private:
        WindowManager *_windowManager;
        InputManager *_inputManager;

};
