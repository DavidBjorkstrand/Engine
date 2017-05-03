#pragma once

class Mesh;
class Scene;
class SoftBody;

struct RenderCommand;
struct Vertex;


#include "engine/scene/entity/component/Component.h"

#include <vector>

#include <GL\glew.h>

using namespace std;

class Cloth : public Component
{
	private:
		GLint _width;
		GLint _height;
		float _spacing;
		SoftBody *_softBody;
		Mesh *_mesh;
		vector<Vertex> *_vertices;
		vector<GLuint> *_indices;

	public:
		Cloth(GLint width, GLint height, float spacing);
		~Cloth();
		void accept(Scene *scene);
		RenderCommand getRenderCommand();
		SoftBody *getSoftBody();

	private:
		void init();
		void computeIndices();
		void computeNormals();
		void setConstraints();
};