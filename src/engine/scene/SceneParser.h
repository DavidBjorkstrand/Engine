#pragma once

#include <vector>

class Mesh;

/*
* This class parses the contents of a scene. 
*/
class SceneParser 
{
	public:
		/*
		* Constructor.
		*/
		SceneParser();

		/*
		* Deconstructor. 
		*/
		~SceneParser();

		/*
		* All visit function are used for scene parsing.
		* There is one for every subclass of Component. 
		*/
		void visit(Mesh *mesh);
};