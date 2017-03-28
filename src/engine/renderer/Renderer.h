#pragma once

#include <vector>

class Shader;
class RenderInformation;

using namespace std;

/*
* Draws engines objects to the screen.
*/
class Renderer
{
	public:
		/*
		* Constructor.
		*/
		Renderer();

		/*
		* Deconstructor. 
		*/
		~Renderer();

		/*
		* Draws the render jobs. 
		*/
		void draw(RenderInformation *renderInformation);

	private:
		Shader *_shader;

};