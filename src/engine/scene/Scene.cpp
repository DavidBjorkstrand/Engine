#include "engine/scene/Scene.h"

#include "engine/scene/SceneParser.h"
#include "engine/scene/entity/Entity.h"
#include "engine/scene/entity/component/Behaviour.h"
#include "engine/renderer/RenderInformation.h"

#include <string>
#include <vector>

using namespace std;

Scene::Scene(string sceneName)
{
	_name = sceneName;
    _entities = new vector<Entity*>();
	_sceneParser = new SceneParser();
}

Scene::~Scene()
{
	delete _entities;
}

string Scene::getName()
{
	return _name;
}

void Scene::addEntity(Entity *entity)
{
    _entities->push_back(entity);
}

void Scene::runBehaviours()
{
	_sceneParser->parse(_entities);

	for (Behaviour *behaviour : *_sceneParser->getBehaviours())
	{
		behaviour->update();
	}
}

RenderInformation *Scene::getRenderInformation()
{
	_sceneParser->parse(_entities);

	return _sceneParser->getRenderInformation();
}