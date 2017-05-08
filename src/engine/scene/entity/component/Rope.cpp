#include "engine/scene/entity/component/Rope.h"

#include "engine/scene/Scene.h"
#include "engine/physics/SoftBody.h"
#include "engine/renderer/RenderSystem.h"
#include "engine/scene/entity/component/Mesh.h"

#include <vector>

#include <GL\glew.h>

Rope::Rope()
{
	_softBody = new SoftBody(glm::vec3(0.0f), 80, 10.0f);
	_vertices = new vector<Vertex>();
	_indices = new vector<GLuint>();
	_mesh = new Mesh(GL_DYNAMIC_DRAW);
	_mesh->setDrawMode(GL_LINES);
	_mesh->setMaterial("cloth");

	for (GLuint i = 0; i < 80; i++)
	{
		Vertex vertex;
		vertex.position = glm::vec3(float(i)*0.5f, 40.0f, 0.0f);
		vertex.texCoords = glm::vec2(0.0f);
		vertex.normal = glm::vec3(0.0f);

		_vertices->push_back(vertex);
		_indices->push_back(i);

		if (i != 79)
		{
			_indices->push_back(i + 1);
		}
		
		_softBody->createParticle(vertex.position, glm::vec3(0.0f, 0.0f, 0.0f));

	}

	for (GLuint i = 1; i < 80; i++)
	{
		_softBody->addDistanceConstraint(i - 1, i, 0.5f);
	}
}

void Rope::accept(Scene *scene)
{
	scene->visit(this);
}

SoftBody *Rope::getSoftBody()
{
	return _softBody;
}

RenderCommand Rope::getRenderCommand()
{
	RenderCommand renderCommand;

	for (ParticleSystem::iterator it = _softBody->begin(); it != _softBody->end(); it++)
	{
		(*_vertices)[it->index].position = it->position;
	}

	_mesh->updateBuffers(_vertices, _indices);

	renderCommand.mesh = _mesh;
	renderCommand.modelMatrix = glm::mat4();

	return renderCommand;
}