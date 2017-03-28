#include "Engine.h"
#include "InputManager.h"
#include "interfaces/Input.h"
#include "interfaces\Window.h"
#include "WindowManager.h"
#include "scene/Scene.h"
#include "scene/entity/Entity.h"
#include "renderer\Renderer.h"
#include "renderer\RenderInformation.h"
#include "scene\entity\component\Camera.h"
#include "scene\entity\Transform.h"
#include "..\behviour\CameraController.h"

#include <map>
#include <vector>

#include <GLFW\glfw3.h>

Engine::Engine() 
{
    _inputManager = new InputManager();
    Input::init(_inputManager);

    _windowManager = new WindowManager(_inputManager);
	Window::init(_windowManager);

	_scenes = new map<string, Scene *>();

	_renderer = new Renderer();
}

Engine::~Engine()
{
    delete _windowManager;
    delete _inputManager;
	delete _scenes;
	delete _renderer;
}

void Engine::addScene(Scene *scene)
{
	if (_scenes->empty())
	{
		_activeScene = scene;
	}

	_scenes->insert(pair<string, Scene *>(scene->getName(), scene));
}

void Engine::run() 
{
    while (!Input::checkKey(GLFW_KEY_ESCAPE)) 
    {
		RenderInformation *renderInformation;

        _inputManager->pollEvents();

		_activeScene->runBehaviours();

		renderInformation = _activeScene->getRenderInformation();
		_renderer->draw(renderInformation);

        _windowManager->show();
    }
}

int main(int argc, char *argv[])
{

    Engine *engine = new Engine();
	Scene *scene = new Scene("TheScene");
	Entity *entity = Entity::createPrimitive(PrimitiveTypes::Plane);
	Camera *camera = new Camera(45.0f, 0.1f, 100.0f);
	CameraController *cameraController = new CameraController();
	Entity *cameraEntity = new Entity();

	scene->addEntity(entity);

	cameraEntity->addComponent(camera);
	cameraEntity->addComponent(cameraController);
	cameraEntity->getTransform()->setPosition(glm::vec3(0.0f, 0.0f, 2.0f));

	scene->addEntity(cameraEntity);

	engine->addScene(scene);

    engine->run();

    delete engine;
	delete scene;

    return 0;
}