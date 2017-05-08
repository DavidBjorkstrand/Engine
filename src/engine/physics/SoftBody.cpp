#include "engine/physics/SoftBody.h"

#include "engine/physics/ParticleSystem.h"
#include "engine/physics/PhysicsSystem.h"
#include "engine/physics/ConstraintSolver.h"

#include <GL/glew.h>

SoftBody::SoftBody(glm::vec3 position, GLint numParticles, float particleMass)
	: ParticleSystem(numParticles, particleMass, 0.1f)
{
	_position = position;
	_springConstraints = new vector<SpringConstraint>();
	_distanceConstraints = new vector<DistanceConstraint>();
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
	delete _springConstraints;
}

void SoftBody::addSpringConstraint(GLuint particleIndexI, GLuint particleIndexJ, float length, float ks, float kd)
{
	SpringConstraint constraint;
	Particle *i = getParticle(particleIndexI);
	Particle *j = getParticle(particleIndexJ);

	constraint.i = i;
	constraint.j = j;
	constraint.length = length;
	constraint.ks = ks;
	constraint.kd = kd;

	_springConstraints->push_back(constraint);
}

void SoftBody::addDistanceConstraint(GLuint particleIndexI, GLuint particleIndexJ, float length)
{
	DistanceConstraint distanceConstraint;
	Particle *i = getParticle(particleIndexI);
	Particle *j = getParticle(particleIndexJ);

	distanceConstraint.i = i;
	distanceConstraint.j = j;
	distanceConstraint.length = length;

	_distanceConstraints->push_back(distanceConstraint);
}

vector<SpringConstraint> *SoftBody::getSpringConstraints()
{
	return _springConstraints;
}

vector<DistanceConstraint> *SoftBody::getDistanceConstraints()
{
	return _distanceConstraints;
}