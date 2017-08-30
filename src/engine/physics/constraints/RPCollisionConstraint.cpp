#include "engine/physics/constraint/RPCollisionConstraint.h"

#include "engine/physics/Rigidbody.h"
#include "engine/physics/ParticleSystem.h"
#include "engine/physics/collision/Collider.h"

#include <glm/gtc/type_ptr.hpp>

RPCollisionConstraint::RPCollisionConstraint(Rigidbody *r, Particle *p, glm::vec3 normal, float overlap)
{
	_r = r;
	_p = p;
	_normal = normal;
	_overlap = overlap;
}

float RPCollisionConstraint::b(float dt, float d)
{
	float c1 = 4.0f / (dt * (1 + 4 * d));
	float c2 = 1 / (1 + 4 * d);

	return -c1*_overlap + c2*Gv();
}

float RPCollisionConstraint::Gv()
{
	return glm::dot(_normal, _r->getVelocity()) + glm::dot(-_normal, _p->velocity);
}

float RPCollisionConstraint::Dii()
{
	return _r->getInverseMass()*glm::dot(_normal, _normal) + _p->inverseMass*glm::dot(-_normal, -_normal);
}

void RPCollisionConstraint::updateVelocity(float deltaLambda)
{
	glm::vec3 currentVelocity1 = _r->getVelocity();
	glm::vec3 currentVelocity2 = _p->velocity;

	currentVelocity1 += _r->getInverseMass()*_normal*deltaLambda;
	currentVelocity2 += _p->inverseMass*-_normal*deltaLambda;

	_r->setVelocity(currentVelocity1);
	_p->velocity = currentVelocity2;
}

bool RPCollisionConstraint::complementaryCondition()
{
	return true;
}