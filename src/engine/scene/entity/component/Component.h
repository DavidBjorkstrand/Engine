#pragma once

class SceneParser;
class Entity;

/*
* Base class for components. Components describes a Entity. 
*/
class Component
{
	friend class Entity;

	public:
		/*
		* Used for scene parsing. 
		* Shall be implemented as parser->visist(this); in each subclass. 
		*/
		virtual void accept(SceneParser *parser) = 0;

		/*
		* Gets the entity the component belongs to.
		*/
		Entity* getEntity();

	private:
		Entity *_entity;
};