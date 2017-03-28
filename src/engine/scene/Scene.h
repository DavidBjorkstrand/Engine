#pragma once

#include <vector>
#include <string>

class Entity;

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
		* Adds a Entity to the scene. 
		*/
        void addEntity(Entity *entity);

		/*
		* Traverses scene graph and gathers scene data. 
		*/
		void traverse();

    private:
		string _name;
        vector<Entity*> *_entities;
};