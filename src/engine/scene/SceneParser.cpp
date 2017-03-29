#include "engine/scene/SceneParser.h"

#include "engine/scene/MatrixStack.h"
#include "engine/scene/entity/Entity.h"
#include "engine/scene/entity/Transform.h"
#include "engine/scene/entity/component/Component.h"
#include "engine/scene/entity/component/Mesh.h"
#include "engine/scene/entity/component/Camera.h"
#include "engine/scene/entity/component/Behaviour.h"
#include "engine/renderer/RenderInformation.h"

#include <vector>

SceneParser::SceneParser() 
{
	_renderJobs = new vector<RenderJob *>();
	_cameras = new vector<Camera *>();
	_behaviours = new vector<Behaviour *>();
	_pointLights = new vector<PointLight *>();
	_matrixStack = new MatrixStack();
}

SceneParser::~SceneParser()
{
	delete _renderJobs;
	delete _matrixStack;
}

RenderInformation *SceneParser::getRenderInformation()
{
	return new RenderInformation(_cameras, _renderJobs, _pointLights);
}

vector<Behaviour *> *SceneParser::getBehaviours()
{
	return _behaviours;
}

void SceneParser::parse(vector<Entity *> *root)
{
	_renderJobs->clear();
	_cameras->clear();
	_behaviours->clear();
	_pointLights->clear();

	traverse(root);
}

void SceneParser::visit(Mesh *mesh)
{
	RenderJob *renderJob = mesh->getRenderJob();

	_renderJobs->push_back(renderJob);
}

void SceneParser::visit(Camera *camera)
{
	_cameras->push_back(camera);
}

void SceneParser::visit(Behaviour *behaviour)
{
	_behaviours->push_back(behaviour);
}

void SceneParser::visit(PointLight *pointLight)
{
	_pointLights->push_back(pointLight);
}

void SceneParser::traverse(vector<Entity *> *root)
{
	for (Entity *entity : *root)
	{
		entity->getTransform()->setParentTransform(_matrixStack->top());

		_matrixStack->pushMatrix();
		_matrixStack->applyMatrix(entity->getTransform()->getMatrix());

		for (Component *component : *entity->getComponents())
		{
			component->accept(this);
		}

		traverse(entity->getChildren());

		_matrixStack->popMatrix();
	}
}