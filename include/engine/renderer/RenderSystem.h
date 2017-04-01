#pragma once

#include <vector>

#include <glm/gtc/type_ptr.hpp>

class Mesh;
class Shader;
class Scene;

using namespace std;

struct RenderCommand
{
	Mesh *mesh;
	glm::mat4 modelMatrix;
};

/*
* Vertex information struct.
*/
struct Vertex
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texCoords;
};

/*
* Draws engines objects to the screen.
*/
class RenderSystem
{
	public:
		/*
		* Constructor.
		*/
		RenderSystem();

		/*
		* Deconstructor. 
		*/
		~RenderSystem();

		void setScene(Scene *scene);

		/*
		* Draws the render jobs. 
		*/
		void draw();

	private:
		Scene *_scene;
};