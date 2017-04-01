#pragma once

#include <map>
#include <string>

using namespace std;

class WindowSystem;
class InputSystem;
class ResourceSystem;
class Scene;
class RenderSystem;

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
        WindowSystem *_windowSystem;
        InputSystem *_inputSystem;
		ResourceSystem *_resourceSystem;
		RenderSystem *_renderSystem;
		Scene *_activeScene;
		map<string, Scene *> *_scenes;
		


};
