#include "engine/physics/SoftBody.h"

#include "engine/physics/ParticleSystem.h"
#include "engine/physics/PhysicsSystem.h"
#include "engine/physics/constraint/ConstraintSolver.h"
#include "engine/physics/constraint/Constraint.h"
#include "engine/physics/constraint/PDistanceConstraint.h"
#include "engine/physics/constraint/PPDistanceConstraint.h"

#include <GL/glew.h>

SoftBody::SoftBody(glm::vec3 position, GLint numParticles, float particleMass)
	: ParticleSystem(numParticles, particleMass, 0.1f)
{
	_position = position;
	_springConstraints = new vector<SpringConstraint>();
	_constraints = new vector<Constraint *>();
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

void SoftBody::addDistanceConstraint(GLuint particleIndex, glm::vec3 position, float length)
{
	PDistanceConstraint *constraint = new PDistanceConstraint(getParticle(particleIndex), position, length);

	_constraints->push_back(constraint);
}

void SoftBody::addDistanceConstraint(GLuint particleIndexI, GLuint particleIndexJ, float length)
{
	Particle *i = getParticle(particleIndexI);
	Particle *j = getParticle(particleIndexJ);
	PPDistanceConstraint *constraint = new PPDistanceConstraint(i, j, length); 

	_constraints->push_back(constraint);
}

vector<SpringConstraint> *SoftBody::getSpringConstraints()
{
	return _springConstraints;
}

vector<Constraint *> *SoftBody::getConstraints()
{
	return _constraints;
}