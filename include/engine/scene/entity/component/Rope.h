#pragma once

#include "engine/scene/entity/component/Component.h"

#include <vector>

#include <GL\glew.h>

class SoftBody;
class Mesh;
class Scene;

struct RenderCommand;
struct Vertex;

using namespace std;

class Rope : public Component
{
	private:
		SoftBody *_softBody;
		Mesh *_mesh;
		vector<Vertex> *_vertices;
		vector<GLuint> *_indices;

	public:
		Rope();
		void accept(Scene *scene);
		void onAttach();
		SoftBody *getSoftBody();
		RenderCommand getRenderCommand();
};