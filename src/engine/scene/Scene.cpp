#include "Scene.h"
#include "entity/Entity.h"

#include <string>
#include <vector>

using namespace std;

Scene::Scene(string sceneName)
{
	_name = sceneName;
    _entities = new vector<Entity*>();
}

Scene::~Scene()
{
	delete _entities;
}

void Scene::addEntity(Entity *entity)
{
    _entities->push_back(entity);
}

void Scene::traverse()
{

}