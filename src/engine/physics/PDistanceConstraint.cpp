#include "engine/physics/PDistanceConstraint.h"

#include "engine/physics/ParticleSystem.h"
#include "engine/physics/Collider.h"

#include <iostream>

#include <glm/gtc/type_ptr.hpp>

PDistanceConstraint::PDistanceConstraint(Particle *p, glm::vec3 position, float length)
{
	_p = p;
	_position = position;
	_length = length;
}

float PDistanceConstraint::b(float dt, float d)
{
	float c1 = 4.0f / (dt * (1 + 4 * d));
	float c2 = 1 / (1 + 4 * d);
	float q = glm::length(_p->position - _position) - _length;

	return -c1*q + c2*Gv();
}

float PDistanceConstraint::Gv()
{
	return glm::dot(glm::normalize(_p->position - _position), _p->velocity);
}

float PDistanceConstraint::Dii()
{
	return _p->inverseMass*glm::dot(glm::normalize(_p->position - _position), glm::normalize(_p->position - _position));
}

void PDistanceConstraint::updateVelocity(float deltaLambda)
{
	glm::vec3 currentVelocity = _p->velocity;

	currentVelocity += _p->inverseMass*glm::normalize(_p->position - _position)*deltaLambda;

	_p->velocity = currentVelocity;
}

bool PDistanceConstraint::complementaryCondition()
{
	return false;
}