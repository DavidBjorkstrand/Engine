#include "engine/scene/entity/component/Cloth.h"

#include "engine/scene/Scene.h"
#include "engine/scene/entity/Entity.h"
#include "engine/scene/entity/Transform.h"
#include "engine/scene/entity/component/Mesh.h"
#include "engine/renderer/RenderSystem.h"
#include "engine/physics/SoftBody.h"

#include <vector>
#include <iostream>

#include <GL\glew.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>

Cloth::Cloth(GLint width, GLint height, float spacing)
{
	_width = width;
	_height = height;
	_spacing = spacing;

	init();
	computeIndices();
	computeNormals();
	setConstraints();

	_mesh = new Mesh(_vertices, _indices, "cloth", GL_DYNAMIC_DRAW);
}

Cloth::~Cloth()
{
	delete _softBody;
	delete _mesh;
}

void Cloth::accept(Scene *scene)
{
	scene->visit(this);
}

void Cloth::onAttach()
{

}

void Cloth::attach(int column, int row, glm::vec3 position, float length)
{
	_softBody->addDistanceConstraint(column*_height + row, position, length);
}

RenderCommand Cloth::getRenderCommand()
{
	RenderCommand renderCommand;

	/*GLuint count = 0;
	GLint startColumn = -(_width / 2);
	GLint endColumn = _width + startColumn - 1;
	for (GLint i = startColumn; i <= endColumn; i++)
	{
		_softBody->getParticle(count)->position = glm::vec3(i*0.1f, 4.1f, -20.0f);
		_softBody->getParticle(count)->velocity = glm::vec3(0.0f);
		count += _height;
	}*/

	for (ParticleSystem::iterator it = _softBody->begin(); it != _softBody->end(); it++)
	{
		(*_vertices)[it->index].position = it->position;
	}
	computeNormals();

	_mesh->updateBuffers(_vertices, _indices);

	renderCommand.mesh = _mesh;
	renderCommand.modelMatrix = glm::mat4();

	return renderCommand;
}

SoftBody *Cloth::getSoftBody()
{
	_softBody->setPosition(getEntity()->getTransform()->getWorldPosition());
	return _softBody;
}

void Cloth::init()
{
	_softBody = new SoftBody(glm::vec3(0.0f), _width * _height, 0.05f);
	_vertices = new vector<Vertex>();
	_indices = new vector<GLuint>((_width * 2) * (_height - 1) + (_height - 2));

	GLint startColumn = -(_width / 2);
	GLint endColumn = _width + startColumn - 1;

	GLint startRow = -(_height / 2);
	GLint endRow = _height + startRow - 1;

	for (GLint c = startColumn; c <= endColumn; c++)
	{
		for (GLint r = startRow; r <= endRow; r++)
		{ 
			Vertex vertex;
			vertex.position = glm::vec3(c*_spacing, 0.0f, r*_spacing);
			vertex.normal = glm::vec3(0.0f, 0.0f, 0.0f);
			vertex.texCoords = glm::vec2((c - startColumn) / _width, 1.0f - ((r - startRow) / _height));

			_vertices->push_back(vertex);

			_softBody->createParticle(vertex.position, glm::vec3(0.0f));
		}
	}
}

void Cloth::computeIndices()
{
	int index = 0;
	for (GLuint r = 0; r < _height - 1; r++)
	{
		if (r % 2 == 0)
		{
			int c;
			for (c = 0; c < _width; c++)
			{
				(*_indices)[index++] = c + (r * _width);
				(*_indices)[index++] = c + (r + 1) * _width;
			}

			if (r != _height - 2)
			{
				(*_indices)[index++] = --c + (r * _width);
			}
		}
		else
		{
			int c;
			for (c = _width - 1; c >= 0; c--)
			{
				(*_indices)[index++] = c + (r * _width);
				(*_indices)[index++] = c + (r + 1) * _width;
			}

			if (r != _height - 2)
			{
				(*_indices)[index++] = ++c + (r * _width);
			}
		}
	}
}

void Cloth::computeNormals()
{
	Vertex v0;
	Vertex v1;
	Vertex v2;
	glm::vec3 normal;
	for (GLuint i = 2; i < _indices->size(); i++)
	{
		v0 = (*_vertices)[(*_indices)[i - 2]];
		v1 = (*_vertices)[(*_indices)[i - 1]];
		v2 = (*_vertices)[(*_indices)[i]];

		normal = glm::cross(v1.position - v0.position, v2.position - v0.position);

		if (i % 2 == 0)
		{
			normal = -normal;
		}

		(*_vertices)[(*_indices)[i - 2]].normal += normal;
		(*_vertices)[(*_indices)[i - 1]].normal += normal;
		(*_vertices)[(*_indices)[i]].normal += normal;
	}

	for (GLuint i = 0; i < _vertices->size(); i++)
	{
		if (glm::length((*_vertices)[i].normal) > 0.0f)
		{
			(*_vertices)[i].normal = glm::normalize((*_vertices)[i].normal);
		}
	}
}

void Cloth::setConstraints()
{
	float kd = 2.0f;
	float distance = _spacing;
	float springForce = 1000.0f;
	for (GLuint r = 0; r < _height; r++)
	{
		for (GLuint c = 0; c < _width; c++)
		{
			GLuint index = c + (r * _width);
			if (c != _width - 1)
			{
				_softBody->addSpringConstraint(index, index + 1, distance, springForce, kd);
			}

			if (r != _height - 1)
			{
				_softBody->addSpringConstraint(index, index + _width, distance, springForce, kd);
			}

			if ((c != _width - 1) && (r != _height - 1))
			{
				_softBody->addSpringConstraint(index, index + _width + 1, glm::sqrt(distance * distance + distance * distance), springForce, kd);
			}

			if ((c != 0) && (r != _height - 1))
			{
				_softBody->addSpringConstraint(index, index + _width - 1, glm::sqrt(distance * distance + distance * distance), springForce, kd);
			}

			if (c <= _width - 1 - 2)
			{
				_softBody->addSpringConstraint(index, index + 2, distance*2.0f, springForce, kd);
			}

			if (r <= _height - 1 - 2)
			{
				_softBody->addSpringConstraint(index, index + _width * 2, distance*2.0f, springForce, kd);
			}
		}
	}
}

