#include "engine/SceneBuilder.h"

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

#include <map>
#include <vector>
#include <windows.h>
#include <iostream>

Scene *SceneBuilder::buildScene1()
{
	Scene *scene = new Scene("TheScene");

	Entity *entity = Entity::createPrimitive(PrimitiveTypes::Sphere);
	entity->getComponent<Mesh>()->setMaterial("rustediron");
	entity->getTransform()->setPosition(glm::vec3(0.0f, 5.0f, -15.0f));
	entity->getTransform()->setScale(glm::vec3(10.0f, 10.0f, 10.0f));
	entity->getComponent<SphereCollider>()->setRadius(5.0f);
	scene->addEntity(entity);

	entity = Entity::createPrimitive(PrimitiveTypes::Plane);
	entity->getComponent<Mesh>()->setMaterial("floor");
	entity->getTransform()->rotate(-90.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	entity->getTransform()->setScale(glm::vec3(500.0f, 500.0f, 1.0f));
	scene->addEntity(entity);

	entity = Entity::createPrimitive(PrimitiveTypes::Plane);
	entity->getComponent<Mesh>()->setMaterial("floor");
	entity->getTransform()->setPosition(glm::vec3(0.0f, 0.0f, -150.0f));
	//entity->getTransform()->rotate(-90.0f, glm::vec3(0.0f, 0.0f, 0.0f));
	entity->getTransform()->setScale(glm::vec3(500.0f, 500.0f, 1.0f));
	scene->addEntity(entity);

	Camera *camera = new Camera(45.0f, 0.1f, 400.0f);
	CameraController *cameraController = new CameraController();
	Entity *cameraEntity = new Entity();
	cameraEntity->addComponent(camera);
	cameraEntity->addComponent(cameraController);
	cameraEntity->getTransform()->setPosition(glm::vec3(60.0f, 50.0f, 85.0f));
	cameraEntity->getTransform()->rotate(-15.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	cameraEntity->getTransform()->rotate(25.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	scene->addEntity(cameraEntity);

	ParticleEmitter *particleEmitter = new ParticleEmitter(0.6f, 1.2f, 0.0025f, 5.0f, 100.0f, 1.0f, 1000, 10.0f);
	Entity *particleEnity = new Entity();
	particleEnity->addComponent(particleEmitter);
	particleEnity->getTransform()->rotate(17.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	particleEnity->getTransform()->setPosition(glm::vec3(0.0f, 2.0f, 450.0f));
	scene->addEntity(particleEnity);

	return scene;
}

Scene *SceneBuilder::buildScene2()
{
	Scene *scene = new Scene("TheScene");

	Entity *entity = Entity::createPrimitive(PrimitiveTypes::Sphere);
	entity->getComponent<Mesh>()->setMaterial("rustediron");
	entity->getTransform()->setPosition(glm::vec3(0.0f, 5.0f, -15.0f));
	entity->getTransform()->setScale(glm::vec3(10.0f, 10.0f, 10.0f));
	entity->getComponent<SphereCollider>()->setRadius(5.0f);
	scene->addEntity(entity);

	entity = Entity::createPrimitive(PrimitiveTypes::Plane);
	entity->getComponent<Mesh>()->setMaterial("floor");
	entity->getTransform()->rotate(-90.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	entity->getTransform()->setScale(glm::vec3(500.0f, 500.0f, 1.0f));
	scene->addEntity(entity);

	entity = Entity::createPrimitive(PrimitiveTypes::Plane);
	entity->getComponent<Mesh>()->setMaterial("floor");
	entity->getTransform()->setPosition(glm::vec3(0.0f, 0.0f, -150.0f));
	//entity->getTransform()->rotate(-90.0f, glm::vec3(0.0f, 0.0f, 0.0f));
	entity->getTransform()->setScale(glm::vec3(500.0f, 500.0f, 1.0f));
	scene->addEntity(entity);

	Camera *camera = new Camera(45.0f, 0.1f, 400.0f);
	CameraController *cameraController = new CameraController();
	Entity *cameraEntity = new Entity();
	cameraEntity->addComponent(camera);
	cameraEntity->addComponent(cameraController);
	cameraEntity->getTransform()->setPosition(glm::vec3(60.0f, 50.0f, 85.0f));
	cameraEntity->getTransform()->rotate(-15.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	cameraEntity->getTransform()->rotate(25.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	scene->addEntity(cameraEntity);

	Cloth *cloth = new Cloth(41, 41, 1.0f);
	Entity *clothEntity = new Entity();
	clothEntity->getTransform()->setPosition(glm::vec3(0.0f, 41.0f, -100.0f));
	clothEntity->addComponent(cloth);
	scene->addEntity(clothEntity);

	return scene;
}

Scene *SceneBuilder::buildScene3()
{
	Scene *scene = new Scene("TheScene");

	Entity *entity = Entity::createPrimitive(PrimitiveTypes::Sphere);
	entity->getComponent<Mesh>()->setMaterial("rustediron");
	entity->getTransform()->setPosition(glm::vec3(0.0f, 5.0f, -15.0f));
	entity->getTransform()->setScale(glm::vec3(10.0f, 10.0f, 10.0f));
	entity->getComponent<SphereCollider>()->setRadius(5.0f);
	scene->addEntity(entity);

	entity = Entity::createPrimitive(PrimitiveTypes::Plane);
	entity->getComponent<Mesh>()->setMaterial("floor");
	entity->getTransform()->rotate(-90.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	entity->getTransform()->setScale(glm::vec3(500.0f, 500.0f, 1.0f));
	scene->addEntity(entity);

	entity = Entity::createPrimitive(PrimitiveTypes::Plane);
	entity->getComponent<Mesh>()->setMaterial("floor");
	entity->getTransform()->setPosition(glm::vec3(0.0f, 0.0f, -150.0f));
	//entity->getTransform()->rotate(-90.0f, glm::vec3(0.0f, 0.0f, 0.0f));
	entity->getTransform()->setScale(glm::vec3(500.0f, 500.0f, 1.0f));
	scene->addEntity(entity);

	Camera *camera = new Camera(45.0f, 0.1f, 400.0f);
	CameraController *cameraController = new CameraController();
	Entity *cameraEntity = new Entity();
	cameraEntity->addComponent(camera);
	cameraEntity->addComponent(cameraController);
	cameraEntity->getTransform()->setPosition(glm::vec3(60.0f, 50.0f, 85.0f));
	cameraEntity->getTransform()->rotate(-15.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	cameraEntity->getTransform()->rotate(25.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	scene->addEntity(cameraEntity);

	Rope *rope = new Rope();
	Entity *ropeEntity = new Entity();
	ropeEntity->addComponent(rope);
	scene->addEntity(ropeEntity);

	return scene;
}

Scene *SceneBuilder::buildScene4()
{
	Scene *scene = new Scene("TheScene");

	Entity *entity = Entity::createPrimitive(PrimitiveTypes::Sphere);
	entity->getComponent<Mesh>()->setMaterial("rustediron");
	entity->getTransform()->setPosition(glm::vec3(0.0f, 5.0f, -15.0f));
	entity->getTransform()->setScale(glm::vec3(10.0f, 10.0f, 10.0f));
	entity->getComponent<SphereCollider>()->setRadius(5.0f);
	scene->addEntity(entity);

	entity = Entity::createPrimitive(PrimitiveTypes::Sphere);
	entity->getComponent<Mesh>()->setMaterial("rustediron");
	entity->getTransform()->setPosition(glm::vec3(0.0f, 27.0f, -15.0f));
	entity->getTransform()->setScale(glm::vec3(10.0f, 10.0f, 10.0f));
	entity->getComponent<SphereCollider>()->setRadius(5.0f);
	scene->addEntity(entity);

	entity = Entity::createPrimitive(PrimitiveTypes::Sphere);
	entity->getComponent<Mesh>()->setMaterial("rustediron");
	entity->getTransform()->setPosition(glm::vec3(0.0f, 40.0f, -15.0f));
	entity->getTransform()->setScale(glm::vec3(10.0f, 10.0f, 10.0f));
	entity->getComponent<SphereCollider>()->setRadius(5.0f);
	scene->addEntity(entity);

	entity = Entity::createPrimitive(PrimitiveTypes::Sphere);
	entity->getComponent<Mesh>()->setMaterial("rustediron");
	entity->getTransform()->setPosition(glm::vec3(0.0f, 55.0f, -15.0f));
	entity->getTransform()->setScale(glm::vec3(10.0f, 10.0f, 10.0f));
	entity->getComponent<SphereCollider>()->setRadius(5.0f);
	scene->addEntity(entity);

	entity = Entity::createPrimitive(PrimitiveTypes::Sphere);
	entity->getComponent<Mesh>()->setMaterial("rustediron");
	entity->getTransform()->setPosition(glm::vec3(0.0f, 70.0f, -15.0f));
	entity->getTransform()->setScale(glm::vec3(10.0f, 10.0f, 10.0f));
	entity->getComponent<SphereCollider>()->setRadius(5.0f);
	scene->addEntity(entity);

	entity = Entity::createPrimitive(PrimitiveTypes::Sphere);
	entity->getComponent<Mesh>()->setMaterial("rustediron");
	entity->getTransform()->setPosition(glm::vec3(0.0f, 5.0f, -25.0f));
	entity->getTransform()->setScale(glm::vec3(10.0f, 10.0f, 10.0f));
	entity->getComponent<SphereCollider>()->setRadius(5.0f);
	scene->addEntity(entity);

	entity = Entity::createPrimitive(PrimitiveTypes::Sphere);
	entity->getComponent<Mesh>()->setMaterial("rustediron");
	entity->getTransform()->setPosition(glm::vec3(0.0f, 27.0f, -25.0f));
	entity->getTransform()->setScale(glm::vec3(10.0f, 10.0f, 10.0f));
	entity->getComponent<SphereCollider>()->setRadius(5.0f);
	scene->addEntity(entity);

	entity = Entity::createPrimitive(PrimitiveTypes::Sphere);
	entity->getComponent<Mesh>()->setMaterial("rustediron");
	entity->getTransform()->setPosition(glm::vec3(0.0f, 40.0f, -25.0f));
	entity->getTransform()->setScale(glm::vec3(10.0f, 10.0f, 10.0f));
	entity->getComponent<SphereCollider>()->setRadius(5.0f);
	scene->addEntity(entity);

	entity = Entity::createPrimitive(PrimitiveTypes::Sphere);
	entity->getComponent<Mesh>()->setMaterial("rustediron");
	entity->getTransform()->setPosition(glm::vec3(0.0f, 55.0f, -25.0f));
	entity->getTransform()->setScale(glm::vec3(10.0f, 10.0f, 10.0f));
	entity->getComponent<SphereCollider>()->setRadius(5.0f);
	scene->addEntity(entity);

	entity = Entity::createPrimitive(PrimitiveTypes::Sphere);
	entity->getComponent<Mesh>()->setMaterial("rustediron");
	entity->getTransform()->setPosition(glm::vec3(0.0f, 70.0f, -25.0f));
	entity->getTransform()->setScale(glm::vec3(10.0f, 10.0f, 10.0f));
	entity->getComponent<SphereCollider>()->setRadius(5.0f);
	scene->addEntity(entity);

	entity = Entity::createPrimitive(PrimitiveTypes::Plane);
	entity->getComponent<Mesh>()->setMaterial("floor");
	entity->getTransform()->rotate(-90.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	entity->getTransform()->setScale(glm::vec3(500.0f, 500.0f, 1.0f));
	scene->addEntity(entity);

	entity = Entity::createPrimitive(PrimitiveTypes::Plane);
	entity->getComponent<Mesh>()->setMaterial("floor");
	entity->getTransform()->setPosition(glm::vec3(0.0f, 0.0f, -150.0f));
	//entity->getTransform()->rotate(-90.0f, glm::vec3(0.0f, 0.0f, 0.0f));
	entity->getTransform()->setScale(glm::vec3(500.0f, 500.0f, 1.0f));
	scene->addEntity(entity);

	Camera *camera = new Camera(45.0f, 0.1f, 400.0f);
	CameraController *cameraController = new CameraController();
	Entity *cameraEntity = new Entity();
	cameraEntity->addComponent(camera);
	cameraEntity->addComponent(cameraController);
	cameraEntity->getTransform()->setPosition(glm::vec3(60.0f, 50.0f, 85.0f));
	cameraEntity->getTransform()->rotate(-15.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	cameraEntity->getTransform()->rotate(25.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	scene->addEntity(cameraEntity);

	return scene;
}

Scene *SceneBuilder::buildScene5()
{
	Scene *scene = new Scene("TheScene");

	Entity *entity = Entity::createPrimitive(PrimitiveTypes::Sphere);
	entity->getComponent<Mesh>()->setMaterial("rustediron");
	entity->getTransform()->setPosition(glm::vec3(0.0f, 1.0f, 0.0f));
	entity->getTransform()->setScale(glm::vec3(1.0f, 1.0f, 1.0f));
	entity->getComponent<SphereCollider>()->setRadius(0.5f);
	scene->addEntity(entity);

	/*entity = Entity::createPrimitive(PrimitiveTypes::Sphere);
	entity->getComponent<Mesh>()->setMaterial("rustediron");
	entity->getTransform()->setPosition(glm::vec3(0.0f, 2.5f, 0.0f));
	entity->getTransform()->setScale(glm::vec3(1.0f, 1.0f, 1.0f));
	entity->getComponent<SphereCollider>()->setRadius(0.5f);
	scene->addEntity(entity);

	entity = Entity::createPrimitive(PrimitiveTypes::Sphere);
	entity->getComponent<Mesh>()->setMaterial("rustediron");
	entity->getTransform()->setPosition(glm::vec3(0.0f, 4.0f, 0.0f));
	entity->getTransform()->setScale(glm::vec3(1.0f, 1.0f, 1.0f));
	entity->getComponent<SphereCollider>()->setRadius(0.5f);
	scene->addEntity(entity);

	entity = Entity::createPrimitive(PrimitiveTypes::Sphere);
	entity->getComponent<Mesh>()->setMaterial("rustediron");
	entity->getTransform()->setPosition(glm::vec3(0.0f, 5.5f, 0.0f));
	entity->getTransform()->setScale(glm::vec3(1.0f, 1.0f, 1.0f));
	entity->getComponent<SphereCollider>()->setRadius(0.5f);
	scene->addEntity(entity);

	entity = Entity::createPrimitive(PrimitiveTypes::Sphere);
	entity->getComponent<Mesh>()->setMaterial("rustediron");
	entity->getTransform()->setPosition(glm::vec3(0.0f, 7.0f, 0.0f));
	entity->getTransform()->setScale(glm::vec3(1.0f, 1.0f, 1.0f));
	entity->getComponent<SphereCollider>()->setRadius(0.5f);
	scene->addEntity(entity);*/

	entity = Entity::createPrimitive(PrimitiveTypes::Sphere);
	entity->getComponent<Mesh>()->setMaterial("rustediron");
	entity->getTransform()->setPosition(glm::vec3(0.0f, 1.0f, -1.5f));
	entity->getTransform()->setScale(glm::vec3(1.0f, 1.0f, 1.0f));
	entity->getComponent<SphereCollider>()->setRadius(0.5f);
	scene->addEntity(entity);

	entity = Entity::createPrimitive(PrimitiveTypes::Sphere);
	entity->getComponent<Mesh>()->setMaterial("rustediron");
	entity->getTransform()->setPosition(glm::vec3(0.0f, 2.5f, -1.5f));
	entity->getTransform()->setScale(glm::vec3(1.0f, 1.0f, 1.0f));
	entity->getComponent<SphereCollider>()->setRadius(0.5f);
	scene->addEntity(entity);

	entity = Entity::createPrimitive(PrimitiveTypes::Sphere);
	entity->getComponent<Mesh>()->setMaterial("rustediron");
	entity->getTransform()->setPosition(glm::vec3(0.0f, 4.0f, -1.5f));
	entity->getTransform()->setScale(glm::vec3(1.0f, 1.0f, 1.0f));
	entity->getComponent<SphereCollider>()->setRadius(0.5f);
	scene->addEntity(entity);

	entity = Entity::createPrimitive(PrimitiveTypes::Sphere);
	entity->getComponent<Mesh>()->setMaterial("rustediron");
	entity->getTransform()->setPosition(glm::vec3(0.0f, 5.5f, -1.5f));
	entity->getTransform()->setScale(glm::vec3(1.0f, 1.0f, 1.0f));
	entity->getComponent<SphereCollider>()->setRadius(0.5f);
	scene->addEntity(entity);

	entity = Entity::createPrimitive(PrimitiveTypes::Sphere);
	entity->getComponent<Mesh>()->setMaterial("rustediron");
	entity->getTransform()->setPosition(glm::vec3(0.0f, 7.0f, -1.5f));
	entity->getTransform()->setScale(glm::vec3(1.0f, 1.0f, 1.0f));
	entity->getComponent<SphereCollider>()->setRadius(0.5f);
	scene->addEntity(entity);

	entity = Entity::createPrimitive(PrimitiveTypes::Plane);
	entity->getComponent<Mesh>()->setMaterial("floor");
	entity->getTransform()->rotate(-90.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	entity->getTransform()->setScale(glm::vec3(50.0f, 50.0f, 1.0f));
	scene->addEntity(entity);

	entity = Entity::createPrimitive(PrimitiveTypes::Plane);
	entity->getComponent<Mesh>()->setMaterial("floor");
	entity->getTransform()->setPosition(glm::vec3(0.0f, 0.0f, -25.0f));
	//entity->getTransform()->rotate(-90.0f, glm::vec3(0.0f, 0.0f, 0.0f));
	entity->getTransform()->setScale(glm::vec3(50.0f, 50.0f, 1.0f));
	scene->addEntity(entity);

	Camera *camera = new Camera(45.0f, 0.1f, 400.0f);
	CameraController *cameraController = new CameraController();
	Entity *cameraEntity = new Entity();
	cameraEntity->addComponent(camera);
	cameraEntity->addComponent(cameraController);
	cameraEntity->getTransform()->setPosition(glm::vec3(6.0f, 6.0f, 8.0f));
	cameraEntity->getTransform()->rotate(-15.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	cameraEntity->getTransform()->rotate(25.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	scene->addEntity(cameraEntity);

	/*PointLight *pointLight = new PointLight(glm::vec3(300.0f, 300.0f, 300.0f));
	Entity *lightEntity = new Entity();
	lightEntity->addComponent(pointLight);
	lightEntity->getTransform()->setPosition(glm::vec3(0.0f, 12.0f, 0.0f));
	scene->addEntity(lightEntity);*/

	/*ParticleEmitter *particleEmitter = new ParticleEmitter(0.06f, 0.12f, 0.0025f, 5.0f, 10.0f, 0.1f, 1000, 10.0f);
	Entity *particleEnity = new Entity();
	particleEnity->addComponent(particleEmitter);
	particleEnity->getTransform()->rotate(17.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	particleEnity->getTransform()->setPosition(glm::vec3(0.0f, 2.0f, 5.0f));
	scene->addEntity(particleEnity);*/

	Cloth *cloth = new Cloth(41, 41, 0.1f);
	Entity *clothEntity = new Entity();
	clothEntity->getTransform()->setPosition(glm::vec3(0.0f, 4.1f, -20.0f));
	clothEntity->addComponent(cloth);
	scene->addEntity(clothEntity);

	Rope *rope = new Rope();
	Entity *ropeEntity = new Entity();
	ropeEntity->addComponent(rope);
	scene->addEntity(ropeEntity);

	return scene;
}