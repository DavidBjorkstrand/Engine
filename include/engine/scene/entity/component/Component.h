#pragma once

class Scene;
class Entity;

/*
* Base class for components. Components describes a Entity. 
*/
class Component
{
	friend class Entity;

	private:
		Entity *_entity;

	public:
		/*
		* Used for scene parsing. 
		* Shall be implemented as parser->visist(this); in each subclass. 
		*/
		virtual void accept(Scene *scene) = 0;

		/*
		* Gets the entity the component belongs to.
		*/
		Entity* getEntity();

};