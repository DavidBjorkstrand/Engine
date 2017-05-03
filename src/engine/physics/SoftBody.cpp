#include "engine/physics/SoftBody.h"

#include "engine/physics/ParticleSystem.h"
#include "engine/physics/PhysicsSystem.h"

#include <GL/glew.h>

SoftBody::SoftBody(glm::vec3 position, GLint numParticles, float particleMass)
	: ParticleSystem(numParticles, particleMass, 0.1f)
{
	_position = position;
	_constraints = new vector<SpringConstraint>();
}

void SoftBody::setPosition(glm::vec3 position)
{
	if (_position != position)
	{
		for (ParticleSystem::iterator it = begin(); it != end(); it++)
		{
			it->position -= _position;
			it->position += position;
		}

		_position = position;
	}
}

SoftBody::~SoftBody()
{
	delete _constraints;
}

void SoftBody::addConstraint(GLuint particleIndexI, GLuint particleIndexJ, float length, float ks, float kd)
{
	SpringConstraint constraint;
	Particle *i = getParticle(particleIndexI);
	Particle *j = getParticle(particleIndexJ);

	constraint.i = i;
	constraint.j = j;
	constraint.length = length;
	constraint.ks = ks;
	constraint.kd = kd;

	_constraints->push_back(constraint);
}

vector<SpringConstraint> *SoftBody::getConstraints()
{
	return _constraints;
}