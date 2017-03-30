#pragma once

#include "Component.h"

#include <vector>
#include <string>

#include <glm/gtc/type_ptr.hpp>
#include <GL/glew.h>

using namespace std;

class RenderJob;

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
* Class for component that describes a renderble. 
*/
class Mesh : public Component
{
	public:

		/*
		* Constructor. Takes a vector of vertices and a list of indices.
		*/
		Mesh(vector<Vertex> *vertices, vector<GLuint> *indices, string materialName);

		/*
		* Deconstructor. 
		*/
		~Mesh();

		/*
		* Inherited from Component. Used for parsing scene. 
		*/
		void accept(SceneParser *sceneParser);

		/*
		* Sets the material of the mesh. 
		*/
		void setMaterial(string materialName);

		/*
		* Gets a render job for the mesh.
		*/
		RenderJob *getRenderJob();

		/*
		* Factory function that creates a Plane mesh. 
		*/
		static Mesh *createPlane();

		/*
		* Factory function that creates a Sphere mesh.
		*/
		static Mesh *createSphere();

		/*
		* Factory function that creates a cube Mesh.
		*/
		static Mesh *createCube();

	private:
		/*
		* Helper function that creates buffers needed by OpenGL to draw. 
		*/
		void initBuffers(vector<Vertex> *vertices, vector<GLuint> *indices);

		GLuint _VAO;
		GLuint _VBO;
		GLuint _EBO;
		GLuint _nIndices;
		string _materialName;

};