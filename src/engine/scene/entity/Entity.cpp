#include "engine/scene/entity/Entity.h"

#include "engine/scene/entity/Transform.h"
#include "engine/scene/entity/component/Component.h"
#include "engine/scene/entity/component/Mesh.h" 
#include "engine/physics/collision/SphereCollider.h"
#include "engine/physics/collision/PlaneCollider.h"
#include "engine/scene/entity/component/RigidbodyComponent.h"

#include <vector>

using namespace std;

Entity::Entity() 
{
    _transform = new Transform();
    _components = new vector<Component*>();
    _children = new vector<Entity*>();
}

Entity::~Entity()
{
    for (unsigned int i = 0; i < _components->size(); i++)
    {
        delete _components->at(i);
    }

    delete _components;

    delete _children;
}

Transform *Entity::getTransform()
{
    return _transform;
}

vector<Entity*> *Entity::getChildren()
{
	return _children;
}

vector<Component*> *Entity::getComponents()
{
	return _components;
}

void Entity::addComponent(Component *component)
{
	component->_entity = this;

    _components->push_back(component);

	component->onAttach();
}

void Entity::addChild(Entity *entity)
{
    _children->push_back(entity);
}

Entity *Entity::createPrimitive(PrimitiveTypes type)
{
	Entity *entity = new Entity();

	if (type == PrimitiveTypes::Plane) 
	{
		Mesh *mesh = Mesh::createPlane();
		PlaneCollider *planeCollider = new PlaneCollider(glm::vec3(0.0f), 1.0f, 1.0f);

		entity->addComponent(mesh);
		entity->addComponent(planeCollider);
	}
	else if (type == PrimitiveTypes::Sphere)
	{
		Mesh *mesh = Mesh::createSphere();
		SphereCollider *sphereCollider = new SphereCollider(1.0f);
		RigidbodyComponent *rigidbody = new RigidbodyComponent(1.0f);

		entity->addComponent(mesh);
		entity->addComponent(sphereCollider);
		entity->addComponent(rigidbody);
	}
	else if (type == PrimitiveTypes::Cube)
	{
		Mesh *mesh = Mesh::createCube();

		entity->addComponent(mesh);
	}

	return entity;
}