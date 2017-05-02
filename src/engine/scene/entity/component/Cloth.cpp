#include "engine/scene/entity/component/Cloth.h"

#include "engine/scene/Scene.h"
#include "engine/scene/entity/Entity.h"
#include "engine/scene/entity/Transform.h"
#include "engine/scene/entity/component/Mesh.h"
#include "engine/renderer/RenderSystem.h"
#include "engine/physics/ParticleSystem.h"
#include "engine/physics/SpringConstraint.h"

#include <vector>
#include <iostream>

#include <GL\glew.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>

Cloth::Cloth()
{
	_vertices = new vector<Vertex>();
	_indices = new vector<GLuint>((41*2)*(41-1)+(41-2));
	_particles = new vector<Particle>();
	_springConstraints = new vector<SpringConstraint *>();

	float springForce = 50.0f;

	for (float x = -20.0f; x <= 20.0f; x+= 1.0f)
	{
		for (float z = -20.0f; z <= 20.0f; z+= 1.0f)
		{
			Vertex vertex;
			vertex.position = glm::vec3(x, 0.0f, z) + glm::vec3(0.0f, 50.0f, -55.0f);
			vertex.normal = glm::vec3(0.0f, 0.0f, 0.0f);
			vertex.texCoords = glm::vec2((x + 5.0f) / 10.0f, 1.0f - ((z + 5.0f) / 10.0f));

			_vertices->push_back(vertex);

			Particle particle;
			particle.mass = 0.1f;
			particle.radius = 0.1f;
			particle.inverseMass = 1.0f / particle.mass;
			particle.position = vertex.position;
			particle.velocity = glm::vec3(0.0f);
			particle.predictedVelocity = glm::vec3(0.0f);
			particle.force = glm::vec3(0.0f);
			particle.acceleration = glm::vec3(0.0f);
			

			_particles->push_back(particle);
		}
	}

	int index = 0;
	for (GLuint z = 0; z < 40; z++)
	{
		if (z % 2 == 0)
		{
			int x;
			for (x = 0; x < 41; x++)
			{
				(*_indices)[index++] = x + (z * 41);
				(*_indices)[index++] = x + (z * 41) + 41;
			}

			if (z != 41 - 2)
			{
				(*_indices)[index++] = --x + (z * 41);
			}
		}
		else 
		{
			int x;
			for (x = 40; x >= 0; x--)
			{
				(*_indices)[index++] = x + (z * 41);
				(*_indices)[index++] = x + (z * 41) + 41;
			}

			if (z != 41 - 2)
			{
				(*_indices)[index++] = ++x + (z * 41);
			}
		}
	}

	Vertex v0;
	Vertex v1;
	Vertex v2;
	glm::vec3 normal;
	for (GLuint i = 2; i < _indices->size(); i++)
	{
		v0 = (*_vertices)[(*_indices)[i-2]];
		v1 = (*_vertices)[(*_indices)[i-1]];
		v2 = (*_vertices)[(*_indices)[i]];

		normal = glm::cross(v1.position - v0.position, v2.position - v0.position);

		if (i % 2 == 0)
		{
			normal = -normal;
		}

		(*_vertices)[(*_indices)[i-2]].normal += normal;
		(*_vertices)[(*_indices)[i-1]].normal += normal;
		(*_vertices)[(*_indices)[i]].normal += normal;
	}

	for (GLuint i = 0; i < _vertices->size(); i++)
	{
		if (glm::length((*_vertices)[i].normal) > 0.0f)
		{
			(*_vertices)[i].normal = glm::normalize((*_vertices)[i].normal);
		}
	}

	float Ks = 2.0f;
	float distance = 1.0f;
	for (GLuint z = 0; z < 41; z++)
	{
		for (GLuint x = 0; x < 41; x++)
		{
			GLuint index = x + (z * 41);
			if (x != 40)
			{
				Particle *particlei = &(*_particles)[index];
				Particle *particlej = &(*_particles)[index+1];
				SpringConstraint *springConstraint = new SpringConstraint(particlei, particlej, distance, springForce, Ks);

				_springConstraints->push_back(springConstraint);
			}

			if (z != 40)
			{
				Particle *particlei = &(*_particles)[index];
				Particle *particlej = &(*_particles)[index + 41];
				SpringConstraint *springConstraint = new SpringConstraint(particlei, particlej, distance, springForce, Ks);

				_springConstraints->push_back(springConstraint);
			}

			if ((x != 40) && (z != 40))
			{
				Particle *particlei = &(*_particles)[index];
				Particle *particlej = &(*_particles)[index + 42];
				SpringConstraint *springConstraint = new SpringConstraint(particlei, particlej, glm::sqrt(distance*distance + distance*distance), springForce, Ks);

				_springConstraints->push_back(springConstraint);
			}

			if ((x != 0) && (z != 40))
			{
				Particle *particlei = &(*_particles)[index];
				Particle *particlej = &(*_particles)[index + 40];
				SpringConstraint *springConstraint = new SpringConstraint(particlei, particlej, glm::sqrt(distance*distance + distance*distance), springForce, Ks);

				_springConstraints->push_back(springConstraint);
			}

			if (x <= 38)
			{
				Particle *particlei = &(*_particles)[index];
				Particle *particlej = &(*_particles)[index + 2];
				SpringConstraint *springConstraint = new SpringConstraint(particlei, particlej, distance*2.0f, springForce, 1.0f);

				_springConstraints->push_back(springConstraint);
			}

			if (z <= 38)
			{
				Particle *particlei = &(*_particles)[index];
				Particle *particlej = &(*_particles)[index + 2*41];
				SpringConstraint *springConstraint = new SpringConstraint(particlei, particlej, distance*2.0f, springForce, 1.0f);

				_springConstraints->push_back(springConstraint);
			}
		}
	}

	_mesh = new Mesh(_vertices, _indices, "cloth", GL_DYNAMIC_DRAW);
}

void Cloth::accept(Scene *scene)
{
	scene->visit(this);
}

RenderCommand Cloth::getRenderCommand()
{
	RenderCommand renderCommand;

	float count = 0.0f;
	for (GLuint i = 0; i < 41; i++)
	{
		(*_particles)[i*41].position = glm::vec3(-20.0f+count, 50.0f, -55.0f);
		(*_particles)[i*41].velocity = glm::vec3(0.0f);

		count += 1.0f;
	}

	for (GLuint i = 0; i < _particles->size(); i++)
	{
		(*_vertices)[i].position = (*_particles)[i].position;
	}

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

	_mesh->updateBuffers(_vertices, _indices);

	renderCommand.mesh = _mesh;
	renderCommand.modelMatrix = glm::mat4();

	return renderCommand;
}

vector<Particle> *Cloth::getParticles()
{
	return _particles;
}

vector<SpringConstraint *> *Cloth::getSpringConstraints()
{
	return _springConstraints;
}
