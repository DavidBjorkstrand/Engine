#pragma once

class Mesh;
class Scene;
class SpringConstraint;

struct RenderCommand;
struct Vertex;
struct Particle;

#include "engine/scene/entity/component/Component.h"

#include <vector>

#include <GL\glew.h>

using namespace std;

class Cloth : public Component
{
	private:
		Mesh *_mesh;
		vector<Vertex> *_vertices;
		vector<GLuint> *_indices;
		vector<Particle> *_particles;
		vector<SpringConstraint *> *_springConstraints;

	public:
		Cloth();
		void accept(Scene *scene);
		RenderCommand getRenderCommand();
		vector<Particle> *getParticles();
		vector<SpringConstraint *> *getSpringConstraints();
};