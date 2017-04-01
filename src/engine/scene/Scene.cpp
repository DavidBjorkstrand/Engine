#include "engine/scene/Scene.h"

#include "engine/scene/MatrixStack.h"
#include "engine/scene/SkyBox.h"
#include "engine/scene/entity/Entity.h"
#include "engine/scene/entity/Transform.h"
#include "engine/scene/entity/component/Behaviour.h"
#include "engine/scene/entity/component/Mesh.h"
#include "engine/renderer/RenderSystem.h"

#include <string>
#include <vector>

using namespace std;

Scene::Scene(string sceneName)
{
	_name = sceneName;
    _entities = new vector<Entity*>();

	_matrixStack = new MatrixStack();

	_behaviours = new vector<Behaviour *>();
	_cameras = new vector<Camera *>();
	_pointLights = new vector<PointLight *>();
	_renderCommands = new vector<RenderCommand>();
}

Scene::~Scene()
{
	delete _entities;
	delete _skyBox;
	delete _matrixStack;

	delete _behaviours;
	delete _cameras;
	delete _pointLights;
	delete _renderCommands;
}

string Scene::getName()
{
	return _name;
}

void Scene::setSkyBox(SkyBox *skyBox)
{
	_skyBox = skyBox;
}

void Scene::addEntity(Entity *entity)
{
    _entities->push_back(entity);
}

vector<Behaviour *> *Scene::getBehaviours()
{
	return _behaviours;
}

vector<Camera *> *Scene::getCameras()
{
	return _cameras;
}

vector<PointLight *> *Scene::getPointLights()
{
	return _pointLights;
}

vector<RenderCommand> *Scene::getRenderCommands()
{
	if (_skyBox != nullptr)
	{
		RenderCommand renderCommand;

		renderCommand.mesh =_skyBox->getMesh();
		renderCommand.modelMatrix = glm::mat4();

		_renderCommands->push_back(renderCommand);
	}

	return _renderCommands;
}

void Scene::traverse()
{
	_behaviours->clear();
	_cameras->clear();
	_pointLights->clear();
	_renderCommands->clear();

	depthFirst(_entities);
}

void Scene::visit(Behaviour *behaviour)
{
	_behaviours->push_back(behaviour);
}

void Scene::visit(Camera *camera)
{
	_cameras->push_back(camera);
}

void Scene::visit(Mesh *mesh)
{
	RenderCommand renderCommand;

	renderCommand.mesh = mesh;
	renderCommand.modelMatrix = mesh->getEntity()->getTransform()->getMatrix();

	_renderCommands->push_back(renderCommand);
}

void Scene::visit(PointLight *pointLight)
{
	_pointLights->push_back(pointLight);
}

void Scene::depthFirst(vector<Entity*> *entities)
{
	for (Entity *entity : *entities)
	{
		entity->getTransform()->setParentTransform(_matrixStack->top());

		_matrixStack->pushMatrix();
		_matrixStack->applyMatrix(entity->getTransform()->getMatrix());

		for (Component *component : *entity->getComponents())
		{
			component->accept(this);
		}

		depthFirst(entity->getChildren());

		_matrixStack->popMatrix();
	}
}