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
class FluidRenderer;

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
	private:
		Scene *_scene;
		Mesh *_screenAlignedQuad;
		CubeMap *_irradianceMap;
		CubeMap *_preFilterEnvMap;
		Texture *_brdfLut;

		FluidRenderer *_fluidRenderer;

		TextureGenerator *_sceneGenerator;

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
};