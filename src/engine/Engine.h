#pragma once

#include <map>
#include <string>

using namespace std;

class WindowManager;
class InputManager;
class Scene;
class Renderer;

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
		* Adds a scene to the engine.
		*/
		void addScene(Scene *scene);

        /*
        * Starts the game loop
        */
        void run();

    private:
        WindowManager *_windowManager;
        InputManager *_inputManager;
		Scene *_activeScene;
		map<string, Scene *> *_scenes;
		Renderer *_renderer;


};
