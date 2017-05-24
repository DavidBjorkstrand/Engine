#include "engine/scene/entity/component/ParticleEmitter.h"

#include "engine/physics/PhysicsSystem.h"
#include "engine/physics/ParticleSystem.h"
#include "engine/renderer/RenderSystem.h"
#include "engine/renderer/Material.h"
#include "engine/scene/Scene.h"
#include "engine/scene/entity/Entity.h"
#include "engine/scene/entity/Transform.h"
#include "engine/scene/entity/component/Mesh.h"

#include <windows.h>
#include <iostream>
#include <stdlib.h> 
#include <cstdlib>
#include <stdexcept>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/type_ptr.hpp>

ParticleEmitter::ParticleEmitter(float particleRadius, glm::vec2 spawnArea, float spawnRate, float mass, float velocity,
	float velocityDeviation, GLuint maxParticles, float maxLifeTime)
{
	_spawnArea = spawnArea;
	_sphereSpawn = false;
	init(particleRadius, spawnRate, mass, velocity, velocityDeviation, maxParticles, maxLifeTime);
}

ParticleEmitter::ParticleEmitter(float particleRadius, float spawnRadius, float spawnRate, float mass,
	float velocity, float velocityDeviation, GLuint maxParticles, float maxLifeTime)
{
	_spawnRadius = spawnRadius;
	_sphereSpawn = true;
	init(particleRadius, spawnRate, mass, velocity, velocityDeviation, maxParticles, maxLifeTime);
}

ParticleEmitter::~ParticleEmitter()
{
	delete _lifeTime;
	delete _vertices;
	delete _indices;
}

bool ParticleEmitter::hasActiveParticles()
{
	return _particleSystem->getNumActiveParticles() > 0;
}

ParticleSystem *ParticleEmitter::getParticleSystem()
{
	return _particleSystem;
}

RenderCommand ParticleEmitter::getRenderCommand()
{
	RenderCommand renderCommand;

	int i = 0;
	for (ParticleSystem::iterator it = _particleSystem->begin(); it != _particleSystem->end(); it++)
	{
		Particle particle = *it;

		(*_vertices)[i].position = particle.position;
		(*_vertices)[i].normal = glm::vec3(0.0f);
		(*_vertices)[i].texCoords = glm::vec2(0.0f);

		i++;
	}

	_particleMesh->updateBuffers(_vertices, _indices);

	renderCommand.mesh = _particleMesh;
	renderCommand.modelMatrix = glm::mat4();

	return renderCommand;
}

void ParticleEmitter::accept(Scene *scene)
{
	scene->visit(this);
}

void ParticleEmitter::onAttach()
{

}

void ParticleEmitter::init(float particleRadius, float spawnRate, float mass, float velocity,
	float velocityDeviation, GLuint maxParticles, float maxLifeTime)
{
	_vertices = new vector<Vertex>();
	_indices = new vector<GLuint>();
	_particleMesh = new Mesh(GL_DYNAMIC_DRAW);
	Material *material = new Material();
	material->setShader("fluid");
	material->setFloat("radius", particleRadius);
	material->setFloat("uRoughness", 1.0f);
	material->setFloat("uMetallic", 0.0f);
	_particleMesh->setMaterial(material);

	_particleSystem = new ParticleSystem(maxParticles, mass, particleRadius);

	_spawnRate = spawnRate;
	_spawnRest = 0.0f;
	_velocity = velocity;
	_velocityDeviation = velocityDeviation;
	_maxLifeTime = maxLifeTime;
	_lifeTime = new float[maxParticles];
}

void ParticleEmitter::update(float dt)
{
	// Removes particles that has exceded their lifetimes. 
	destroyParticles(dt);

	// Spawns new particles at constant rate. 
	spawnParticles(dt);

}

void ParticleEmitter::destroyParticles(float dt)
{
	ParticleSystem::iterator it;
	for (it = _particleSystem->begin(); it != _particleSystem->end(); it++)
	{
		float lifeTime = _lifeTime[it->index];
		lifeTime += dt;

		if (lifeTime >= _maxLifeTime)
		{
			it = _particleSystem->destroyParticle(it);
			_vertices->erase(_vertices->end()-1);
			_indices->erase(_indices->end()-1);
		}

		_lifeTime[it->index] = lifeTime;
	}
}

void ParticleEmitter::spawnParticles(float dt)
{
	_spawnRest += dt;
	glm::vec3 entityDirection = getEntity()->getTransform()->getWorldDirection();
	glm::vec3 entityPosition = getEntity()->getTransform()->getWorldPosition();

	while ((_spawnRest >= _spawnRate) && !_particleSystem->isFull())
	{
		glm::vec3 velocity = random(_velocity - (_velocityDeviation / 2.0f), _velocity + (_velocityDeviation / 2.0f))*entityDirection;
		float x;
		float z;
		glm::vec3 forward = glm::vec3(0.0f, 0.0f, -1.0f);
		glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
		glm::vec3 t1 = glm::cross(entityDirection, forward);
		glm::vec3 t2 = glm::cross(entityDirection, up);
		glm::vec3 tangent;
		glm::vec3 tangent2;
		glm::vec3 position;

		if (_sphereSpawn)
		{
			float radian = random(0.0f, 2.0f*3.14f);
			float radius = random(0.0f, _spawnRadius);
			x = radius*glm::cos(radian);
			z = radius*glm::sin(radian);
		}
		else
		{
			x = random(-_spawnArea.x / 2.0f, _spawnArea.x / 2.0f);
			z = random(-_spawnArea.y / 2.0f, _spawnArea.y / 2.0f);
		}

		if (glm::length(t1) > glm::length(t2))
		{
			tangent = t1;
		}
		else
		{
			tangent = t2;
		}

		tangent = glm::normalize(tangent);
		tangent2 = glm::normalize(glm::cross(entityDirection, tangent));
		position = x*tangent + z*tangent2 + entityPosition;

		GLuint index = _particleSystem->createParticle(position, velocity);
		_lifeTime[index] = 0.0f;
		_vertices->push_back(Vertex());
		_indices->push_back(_indices->size());

		_spawnRest -= _spawnRate;
	}
}

float ParticleEmitter::random(float min, float max)
{
	return (static_cast <float> (rand()) / static_cast <float> (RAND_MAX))*(max - min) + min;
}