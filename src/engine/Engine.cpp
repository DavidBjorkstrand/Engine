#include "engine/Engine.h"

#include "engine/ResourceSystem.h"
#include "engine/InputSystem.h"
#include "engine/WindowSystem.h"
#include "engine/interface/Resources.h"
#include "engine/interface/Input.h"
#include "engine/interface/Window.h"
#include "engine/renderer/RenderSystem.h"
#include "engine/physics/PhysicsSystem.h"
#include "engine/physics/SphereCollider.h"
#include "engine/scene/SkyBox.h"
#include "engine/scene/Scene.h"
#include "engine/scene/entity/Entity.h"
#include "engine/scene/entity/Transform.h"
#include "engine/scene/entity/component/Mesh.h"
#include "engine/scene/entity/component/Camera.h"
#include "engine/scene/entity/component/PointLight.h"
#include "engine/scene/entity/component/Behaviour.h"
#include "engine/scene/entity/component/ParticleEmitter.h"
#include "engine/scene/entity/component/Cloth.h"
#include "engine/scene/entity/component/Rope.h"
#include "behaviour/CameraController.h"
#include "engine/SceneBuilder.h"

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
	float currentTime = time.wSecond + time.wMilliseconds / 1000.0f;
	float newTime;
	float dt;
	float printFps = 0.0f;
	int samples = 0;
	float totalFps = 0.0f;

    while (!Input::checkKey(GLFW_KEY_ESCAPE)) 
    {
		GetSystemTime(&time);
		newTime = time.wSecond + time.wMilliseconds / 1000.0f;
		dt = (newTime - currentTime);
		currentTime = newTime;

		if (dt <= 0.0f)
		{
			cout << "Weird error" << endl;
			dt = 0.0f;
		}


		printFps += dt;
		totalFps += (1.0f / dt);
		samples++;

		if (printFps >= 1.0f)
		{
			cout << totalFps / samples << " fps" << endl;
			printFps = 0.0f;
			totalFps = 0.0f;
			samples = 0;
		}

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
	SceneBuilder *sceneBuilder = new SceneBuilder();
	Scene *scene = sceneBuilder->buildScene5();
	SkyBox *skyBox = new SkyBox("resources\\textures\\malibu3k.hdr");

	scene->setSkyBox(skyBox);

	engine->addScene(scene);

    engine->run();

    delete engine;
	delete scene;

    return 0;
}