#include "engine/scene/Scene.h"

#include "engine/scene/MatrixStack.h"
#include "engine/scene/SkyBox.h"
#include "engine/scene/entity/Entity.h"
#include "engine/scene/entity/Transform.h"
#include "engine/scene/entity/component/Behaviour.h"
#include "engine/scene/entity/component/Mesh.h"
#include "engine/scene/entity/component/ParticleEmitter.h"
#include "engine/scene/entity/component/Cloth.h"
#include "engine/scene/entity/component/Rope.h"
#include "engine/scene/entity/component/RigidbodyComponent.h"
#include "engine/renderer/RenderSystem.h"
#include "engine/physics/PhysicsSystem.h"
#include "engine/physics/ParticleSystem.h"
#include "engine/physics/collision/SphereCollider.h"
#include "engine/physics/collision/Collider.h"
#include "engine/physics/SoftBody.h"
#include "engine/physics/Rigidbody.h"

#include <string>
#include <iostream>
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
	_particleSystems = new vector<ParticleSystem *>();
	_colliders = new vector<Collider *>();
	_softBodies = new vector<SoftBody *>();
	_rigidbodies = new vector<Rigidbody *>();
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
	delete _particleSystems;
	delete _colliders;
	delete _softBodies;
	delete _rigidbodies;
}

string Scene::getName()
{
	return _name;
}

void Scene::setSkyBox(SkyBox *skyBox)
{
	_skyBox = skyBox;
}

SkyBox *Scene::getSkyBox()
{
	return _skyBox;
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
		RenderCommand renderCommand = RenderCommand();

		renderCommand.mesh =_skyBox->getMesh();
		renderCommand.modelMatrix = glm::mat4();

		_renderCommands->push_back(renderCommand);
	}

	return _renderCommands;
}

vector<ParticleSystem *> *Scene::getParticleSystems()
{
	return _particleSystems;
}

vector<Collider *> *Scene::getColliders()
{
	return _colliders;
}

vector<SoftBody *> *Scene::getSoftBodies()
{
	return _softBodies;
}

vector<Rigidbody *> *Scene::getRigidbodies()
{
	return _rigidbodies;
}

void Scene::traverse()
{
	_behaviours->clear();
	_cameras->clear();
	_pointLights->clear();
	_renderCommands->clear();
	_particleSystems->clear();
	_colliders->clear();
	_softBodies->clear();
	_rigidbodies->clear();

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
	RenderCommand renderCommand = RenderCommand();

	renderCommand.mesh = mesh;
	renderCommand.modelMatrix = mesh->getEntity()->getTransform()->getMatrix();

	_renderCommands->push_back(renderCommand);
}

void Scene::visit(PointLight *pointLight)
{
	_pointLights->push_back(pointLight);
}

void Scene::visit(ParticleEmitter *particleEmitter)
{
	if (particleEmitter->hasActiveParticles())
	{
		RenderCommand renderCommand = particleEmitter->getRenderCommand();
		_renderCommands->push_back(renderCommand);

		ParticleSystem *particleSystem = particleEmitter->getParticleSystem();
		_particleSystems->push_back(particleSystem);

	}

	_behaviours->push_back(particleEmitter);
}

void Scene::visit(Collider *collider)
{
	_colliders->push_back(collider);
}

void Scene::visit(Cloth *cloth)
{
	_renderCommands->push_back(cloth->getRenderCommand());

	_softBodies->push_back(cloth->getSoftBody());
}

void Scene::visit(Rope *rope)
{
	_renderCommands->push_back(rope->getRenderCommand());

	_softBodies->push_back(rope->getSoftBody());
}

void Scene::visit(RigidbodyComponent *rigidbodyComponent)
{
	glm::vec3 position = rigidbodyComponent->getEntity()->getTransform()->getWorldPosition();
	rigidbodyComponent->getRigidbody()->setPosition(position);
	_rigidbodies->push_back(rigidbodyComponent->getRigidbody());
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