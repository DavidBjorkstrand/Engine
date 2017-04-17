#include "engine/scene/entity/component/ParticleEmitter.h"

#include "engine/physics/PhysicsSystem.h"
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
	delete _particles;
	delete _freeIndexes;
	delete _lifeTime;
	delete _renderCommands;
}

vector<Particle> *ParticleEmitter::getParticles()
{
	return _particles;
}

vector<RenderCommand> *ParticleEmitter::getRenderCommands()
{
	for (GLuint i = 0; i < _particles->size(); i++)
	{
		Particle particle = _particles->at(i);
		glm::mat4 modelMatrix = glm::translate(glm::mat4(), particle.position);

		_renderCommands->at(i).mesh = _particleMesh;
		_renderCommands->at(i).modelMatrix = modelMatrix;
	}

	return _renderCommands;
}

void ParticleEmitter::accept(Scene *scene)
{
	scene->visit(this);
}

void ParticleEmitter::init(float particleRadius, float spawnRate, float mass, float velocity,
	float velocityDeviation, GLuint maxParticles, float maxLifeTime)
{
	_particleMesh = Mesh::createPlane();
	Material *material = new Material();
	material->setShader("fluid");
	material->setFloat("radius", particleRadius);
	material->setFloat("uRoughness", 1.0f);
	material->setFloat("uMetallic", 0.0f);
	_particleMesh->setMaterial(material);
	_particleRadius = particleRadius;
	_spawnRate = spawnRate;
	_spawnRest = 0.0f;
	_mass = mass;
	_inverseMass = 1.0f / _mass;
	_velocity = velocity;
	_velocityDeviation = velocityDeviation;
	_maxParticles = maxParticles;
	_nrParticles = 0.0f;
	_maxLifeTime = maxLifeTime;
	_lifeTime = new float[maxParticles];
	_particles = new vector<Particle>();
	_particles->reserve(maxParticles);
	_freeIndexes = new vector<GLuint>();
	_renderCommands = new vector<RenderCommand>();
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
	vector<Particle>::iterator it;
	for (it = _particles->begin(); it != _particles->end();)
	{
		float lifeTime = _lifeTime[it->index];
		lifeTime += dt;

		if (lifeTime >= _maxLifeTime)
		{
			_freeIndexes->push_back(it->index);

			it = _particles->erase(it);
			_nrParticles--;

			_renderCommands->erase(_renderCommands->begin());
		}
		else
		{
			_lifeTime[it->index] = lifeTime;
			it++;
		}
	}
}

void ParticleEmitter::spawnParticles(float dt)
{
	_spawnRest += dt;
	glm::vec3 entityDirection = getEntity()->getTransform()->getWorldDirection();
	glm::vec3 entityPosition = getEntity()->getTransform()->getWorldPosition();

	while ((_spawnRest >= _spawnRate) && (_nrParticles < _maxParticles))
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

		createParticle(position, velocity);

		_spawnRest -= _spawnRate;
	}
}

void ParticleEmitter::createParticle(glm::vec3 position, glm::vec3 velocity)
{
	Particle particle;

	if (_freeIndexes->empty())
	{
		particle.index = _particles->size();
	}
	else
	{
		particle.index = _freeIndexes->at(0);
		_freeIndexes->erase(_freeIndexes->begin());
	}

	particle.mass = _mass;
	particle.radius = _particleRadius;
	particle.inverseMass = _inverseMass;
	particle.position = position;
	particle.velocity = velocity;
	particle.predictedVelocity = velocity;
	particle.acceleration = glm::vec3(0.0f);
	particle.force = glm::vec3(0.0f);

	_particles->push_back(particle);

	_lifeTime[particle.index] = 0.0f;

	_nrParticles++;

	_renderCommands->push_back(RenderCommand());
}

float ParticleEmitter::random(float min, float max)
{
	return (static_cast <float> (rand()) / static_cast <float> (RAND_MAX))*(max - min) + min;
}