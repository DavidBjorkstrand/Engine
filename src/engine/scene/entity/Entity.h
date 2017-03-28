#pragma once

#include <vector>

using namespace std;

class Transform;
class Component;

enum class PrimitiveTypes { Plane };

/*
* This class represents a object in the world. 
*/
class Entity
{
    public:
        /*
        * Constructor, creates a entity with the Transform component at origin.
        */
        Entity();
        
        /*
        * Deconstructor, deletes the entity and all of its components. Also
        * deletes child vector but does not delete child Entities. 
        */
        ~Entity();

        /*
        * Gets the transform of the Entity.
        */
        Transform *getTransform();

		/*
		* Gets the child Enities of the Entity.
		*/
		vector<Entity*> *getChildren();

		/*
		* Gets the components of the Entity. 
		*/
		vector<Component*> *getComponents();

        /*
        * Adds a component to the entity. 
        */
        void addComponent(Component *component);

        /*
        * Adds a child Entity to the Entity.
        */
        void addChild(Entity *entity);

		/*
		* Creates a Entity with a primitive mesh. 
		* Available types can be found in the enum class PrimitiveTypes. 
		*/
		static Entity *createPrimitive(PrimitiveTypes type);

    private:
        Transform *_transform;
        vector<Component*> *_components;
        vector<Entity*> *_children;
};