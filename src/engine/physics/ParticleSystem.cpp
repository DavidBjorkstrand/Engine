#include "engine/physics/ParticleSystem.h"

#include <glm/gtc/type_ptr.hpp>
#include <GL/glew.h>

ParticleSystem::ParticleSystem(GLuint numParticles, float particleMass, float particleRadius)
{
	_particles = new Particle[numParticles];
	_numParticles = numParticles;
	_numActiveParticles = 0;
	_valid = new GLboolean[numParticles];
	_validRange = 0;
	_particleMass = particleMass;
	_inverseParticleMass = 1.0f / particleMass;
	_particleRadius = particleRadius;
}

ParticleSystem::~ParticleSystem()
{
	delete _particles;
	delete _valid;
}

GLuint ParticleSystem::createParticle(glm::vec3 position, glm::vec3 velocity)
{
	GLint index;

	if (isFull())
	{
		return -1;
	}

	if (_freeIndexQueue.empty())
	{
		index = _validRange + 1;
		_validRange++;
	} 
	else
	{
		index = _freeIndexQueue.front();
		_freeIndexQueue.pop();
	}

	_particles[index].index = index;
	_particles[index].radius = _particleRadius;
	_particles[index].mass = _particleMass;
	_particles[index].inverseMass = _inverseParticleMass;
	_particles[index].position = position;
	_particles[index].velocity = velocity;
	_particles[index].force = glm::vec3(0.0f);
	_particles[index].acceleration = glm::vec3(0.0f);

	_valid[index] = true;

	_numActiveParticles++;

	return index;
}

void ParticleSystem::destroyParticle(GLuint index)
{
	if (index == _validRange)
	{
		_validRange--;
	}
	else 
	{
		_freeIndexQueue.push(index);
	}

	_valid[index] = false;

	_numActiveParticles--;
}

GLuint ParticleSystem::getNumActiveParticles()
{
	return _numActiveParticles;
}

GLboolean ParticleSystem::isFull()
{
	return _freeIndexQueue.empty && (_validRange + 1 == _numParticles);
}

ParticleSystem::iterator ParticleSystem::begin()
{
	return ParticleSystem::iterator(0, _particles, _valid);
}

ParticleSystem::iterator ParticleSystem::end()
{
	return ParticleSystem::iterator(_validRange, _particles, _valid);
}