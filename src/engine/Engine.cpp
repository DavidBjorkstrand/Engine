#include "engine/Engine.h"

#include "engine/ResourceSystem.h"
#include "engine/InputSystem.h"
#include "engine/WindowSystem.h"
#include "engine/interface/Resources.h"
#include "engine/interface/Input.h"
#include "engine/interface/Window.h"
#include "engine/renderer/RenderSystem.h"
#include "engine/physics/PhysicsSystem.h"
#include "engine/scene/SkyBox.h"
#include "engine/scene/Scene.h"
#include "engine/scene/entity/Entity.h"
#include "engine/scene/entity/Transform.h"
#include "engine/scene/entity/component/Mesh.h"
#include "engine/scene/entity/component/Camera.h"
#include "engine/scene/entity/component/PointLight.h"
#include "engine/scene/entity/component/Behaviour.h"
#include "engine/scene/entity/component/ParticleEmitter.h"
#include "behaviour/CameraController.h"

#include <map>
#include <vector>
#include <windows.h>
#include <iostream>

#include <GLFW\glfw3.h>

Engine::Engine() 
{
    _inputSystem = new InputSystem();
    Input::init(_inputSystem);

    _windowSystem = new WindowSystem(_inputSystem);
	Window::init(_windowSystem);

	_resourceSystem = new ResourceSystem();
	Resources::init(_resourceSystem);

	_scenes = new map<string, Scene *>();

	_renderSystem = new RenderSystem();

	_physicsSystem = new PhysicsSystem();
}

Engine::~Engine()
{
    delete _windowSystem;
    delete _inputSystem;
	delete _resourceSystem;
	delete _scenes;
	delete _renderSystem;
}

void Engine::addScene(Scene *scene)
{
	if (_scenes->empty())
	{
		_activeScene = scene;
	}

	_scenes->insert(pair<string, Scene *>(scene->getName(), scene));

	_renderSystem->setScene(_activeScene);

	_physicsSystem->setScene(_activeScene);
}

void Engine::run() 
{
	SYSTEMTIME time;
	GetSystemTime(&time);
	long currentTime = (time.wSecond * 1000) + time.wMilliseconds;
	long newTime;
	float dt;

    while (!Input::checkKey(GLFW_KEY_ESCAPE)) 
    {
		GetSystemTime(&time);
		newTime = (time.wSecond * 1000) + time.wMilliseconds;
		dt = static_cast <float>(newTime - currentTime) / 1000.0f;
		currentTime = newTime;

		//cout << dt << endl;

        _inputSystem->pollEvents();

		_activeScene->traverse();
		for (Behaviour *behaviour : *_activeScene->getBehaviours())
		{
			behaviour->update(dt);
		}

		_physicsSystem->update(dt);

		_renderSystem->draw();

        _windowSystem->show();
    }
}

int main(int argc, char *argv[])
{

    Engine *engine = new Engine();
	Scene *scene = new Scene("TheScene");

	Entity *entity = Entity::createPrimitive(PrimitiveTypes::Sphere);
	entity->getComponent<Mesh>()->setMaterial("rustediron");
	entity->getTransform()->setPosition(glm::vec3(0.0f, 6.0f, -6.0f));
	entity->getTransform()->setScale(glm::vec3(3.0f, 3.0f, 3.0f));
	scene->addEntity(entity);

	entity = Entity::createPrimitive(PrimitiveTypes::Plane);
	entity->getComponent<Mesh>()->setMaterial("floor");
	entity->getTransform()->rotate(-90.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	entity->getTransform()->setScale(glm::vec3(50.0f, 50.0f, 1.0f));
	scene->addEntity(entity);

	Camera *camera = new Camera(45.0f, 0.1f, 100.0f);
	CameraController *cameraController = new CameraController();
	Entity *cameraEntity = new Entity();
	cameraEntity->addComponent(camera);
	cameraEntity->addComponent(cameraController);
	cameraEntity->getTransform()->setPosition(glm::vec3(0.0f, 8.0f, 5.0f));
	cameraEntity->getTransform()->rotate(-30.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	scene->addEntity(cameraEntity);

	PointLight *pointLight = new PointLight(glm::vec3(300.0f, 300.0f, 300.0f));
	Entity *lightEntity = new Entity();
	lightEntity->addComponent(pointLight);
	lightEntity->getTransform()->setPosition(glm::vec3(0.0f, 12.0f, 0.0f));
	scene->addEntity(lightEntity);

	ParticleEmitter *particleEmitter = new ParticleEmitter(0.5f, 1.0f, 0.01f, 0.1f, 50.0f, 10.0f, 1000, 5.0f);
	Entity *particleEnity = new Entity();
	particleEnity->addComponent(particleEmitter);
	particleEnity->getTransform()->rotate(45.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	scene->addEntity(particleEnity);

	SkyBox *skyBox = new SkyBox("resources\\textures\\malibu3k.hdr");

	scene->setSkyBox(skyBox);

	engine->addScene(scene);

    engine->run();

    delete engine;
	delete scene;

    return 0;
}