#include "engine/physics/PPDistanceConstraint.h"

#include "engine/physics/ParticleSystem.h"
#include "engine/physics/Collider.h"

#include <iostream>

#include <glm/gtc/type_ptr.hpp>

PPDistanceConstraint::PPDistanceConstraint(Particle *p1, Particle *p2, float length)
{
	_p1 = p1;
	_p2 = p2;
	_length = length;
}

float PPDistanceConstraint::b(float dt, float d)
{
	float c1 = 4.0f / (dt * (1 + 4 * d));
	float c2 = 1 / (1 + 4 * d);
	float q = glm::length(_p1->position - _p2->position) - _length;

	return -c1*q + c2*Gv();
}

float PPDistanceConstraint::Gv()
{
	return glm::dot(glm::normalize(_p1->position - _p2->position), _p1->velocity) + glm::dot(glm::normalize(_p2->position - _p1->position), _p2->velocity);
}

float PPDistanceConstraint::Dii()
{
	glm::vec3 n = glm::normalize(_p1->position - _p2->position);
	return _p1->inverseMass*glm::dot(n, n) + _p2->inverseMass*glm::dot(-n, -n); 
}

void PPDistanceConstraint::updateVelocity(float deltaLambda)
{
	glm::vec3 currentVelocity1 = _p1->velocity;
	glm::vec3 currentVelocity2 = _p2->velocity;
	glm::vec3 n = glm::normalize(_p1->position - _p2->position);

	currentVelocity1 += _p1->inverseMass*n*deltaLambda;
	currentVelocity2 -= _p2->inverseMass*n*deltaLambda;

	_p1->velocity = currentVelocity1;
	_p2->velocity = currentVelocity2;
}

bool PPDistanceConstraint::complementaryCondition()
{
	return false;
}