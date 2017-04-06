#pragma once

#include <vector>

#include <glm/gtc/type_ptr.hpp>
#include <GL/glew.h>

class Mesh;
class Shader;
class Scene;
class CubeMap;
class Texture;
class TextureGenerator;

using namespace std;

struct RenderCommand
{
	Mesh *mesh;
	GLboolean fluid;
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
		void createIrradianceMap();
		void createPreFilterEnvMap();

		Scene *_scene;
		Mesh *_screenAlignedQuad;
		CubeMap *_irradianceMap;
		CubeMap *_preFilterEnvMap;
		Texture *_brdfLut;

		TextureGenerator *_fluidDepthMapGenerator;
		TextureGenerator *_pingpongGenerators[2];
};