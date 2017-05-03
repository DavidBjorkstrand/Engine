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
		index = _validRange;
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

ParticleSystem::iterator ParticleSystem::destroyParticle(iterator it)
{
	if (it->index == _validRange-1)
	{
		_validRange--;
		it.decrementRange();
	}
	else 
	{
		_freeIndexQueue.push(it->index);
	}

	_valid[it->index] = false;

	_numActiveParticles--;

	return it;
}

Particle *ParticleSystem::getParticle(GLuint index)
{
	return &_particles[index];
}

GLuint ParticleSystem::getNumActiveParticles()
{
	return _numActiveParticles;
}

GLboolean ParticleSystem::isFull()
{
	return _freeIndexQueue.empty() && (_validRange == _numParticles);
}

ParticleSystem::iterator ParticleSystem::begin()
{
	return ParticleSystem::iterator(0, _particles, _valid, _validRange);
}

ParticleSystem::iterator ParticleSystem::end()
{
	return ParticleSystem::iterator(_validRange, _particles, _valid, _validRange);
}