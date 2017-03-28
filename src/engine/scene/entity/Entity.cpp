#include "Entity.h"
#include "Transform.h"
#include "component/Component.h"
#include "component/Mesh.h"

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
    _components->push_back(component);
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

		entity->addComponent(mesh);
	}

	return entity;
}