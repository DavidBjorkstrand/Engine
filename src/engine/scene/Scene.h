#pragma once

#include <vector>
#include <string>

class Entity;
class RenderJob;
class Camera;
class SceneParser;
class RenderInformation;

using namespace std;

/*
* This class represents a scene.
*/
class Scene
{
    public:
		/*
		* Constructor. Creates a empty scene. 
		*/
        Scene(string sceneName);

		/*
		* Deconstructor.
		*/
        ~Scene();

		/*
		* Gets the name of the scene.
		*/
		string getName();

		/*
		* Adds a Entity to the scene. 
		*/
        void addEntity(Entity *entity);

		/*
		* Runs all behaviours in the scene. 
		*/
		void runBehaviours();

		/*
		* Gets render jobs from the scene.
		*/
		RenderInformation *getRenderInformation();

    private:
		string _name;
        vector<Entity*> *_entities;
		SceneParser *_sceneParser;
};