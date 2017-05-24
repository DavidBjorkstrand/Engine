#pragma once

#include "Component.h"

#include <vector>
#include <string>

#include <glm/gtc/type_ptr.hpp>
#include <GL/glew.h>

using namespace std;

struct Vertex;
class Scene;
class Material;

/*
* Class for component that describes a renderble. 
*/
class Mesh : public Component
{
	private:
		vector<Vertex> *_vertices;
		vector<GLuint> *_indices;
		Material *_material;
		GLuint _VAO;
		GLuint _EBO;
		GLuint _VBO;
		GLenum _depthFunc;
		GLenum _usage;
		GLenum _drawMode;

	public:

		/*
		* Constructor. Takes a vector of vertices and a list of indices.
		*/
		Mesh(vector<Vertex> *vertices, vector<GLuint> *indices, string materialName);

		Mesh(vector<Vertex> *vertices, vector<GLuint> *indices, string materialName, GLenum usage);

		Mesh(GLenum usage);

		/*
		* Deconstructor. 
		*/
		~Mesh();

		/*
		* Inherited from Component. Used for parsing scene. 
		*/
		void accept(Scene *scene);

		void onAttach();

		/*
		* Sets the material of the mesh. 
		*/
		void setMaterial(string materialName);

		void setMaterial(Material *material);

		void setDepthFunc(GLenum depthFunc);

		void setDrawMode(GLenum drawMode);

		Material *getMaterial();

		GLuint getVAO();

		GLuint getNIndices();

		GLenum getDepthFunc();

		GLenum getDrawMode();

		void updateBuffers(vector<Vertex> *vertices, vector<GLuint> *indices);

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
		void initBuffers();



};