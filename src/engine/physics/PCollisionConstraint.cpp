#include "engine/physics/PCollisionConstraint.h"

#include "engine/physics/ParticleSystem.h"
#include "engine/physics/Collider.h"

#include <glm/gtc/type_ptr.hpp>

PCollisionConstraint::PCollisionConstraint(Particle *p, glm::vec3 normal, float overlap)
{
	_p = p;
	_normal = normal;
	_overlap = overlap;
}

float PCollisionConstraint::b(float dt, float d)
{
	float c1 = 4.0f / (dt * (1 + 4 * d));
	float c2 = 1 / (1 + 4 * d);

	return -c1*_overlap + c2*Gv();
}

float PCollisionConstraint::Gv()
{
	return glm::dot(_normal, _p->velocity);
}

float PCollisionConstraint::Dii()
{
	return _p->inverseMass*glm::dot(_normal, _normal);
}

void PCollisionConstraint::updateVelocity(float deltaLambda)
{
	glm::vec3 currentVelocity = _p->velocity;

	currentVelocity += _p->inverseMass*_normal*deltaLambda;

	_p->velocity = currentVelocity;
}

bool PCollisionConstraint::complementaryCondition()
{
	return true;
}