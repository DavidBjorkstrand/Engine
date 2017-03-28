#pragma once

class SceneParser;

/*
* Base class for components. Components describes a Entity. 
*/
class Component
{
	/*
	* Used for scene parsing. 
	* Shall be implemented as parser->visist(this); in each subclass. 
	*/
	virtual void accept(SceneParser *parser) = 0;
};