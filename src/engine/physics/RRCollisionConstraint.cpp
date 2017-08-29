#include "engine/physics/RRCollisionConstraint.h"

#include "engine/physics/Rigidbody.h"
#include "engine/physics/Collider.h"

#include <glm/gtc/type_ptr.hpp>

RRCollisionConstraint::RRCollisionConstraint(Rigidbody *r1, Rigidbody *r2, glm::vec3 normal, float overlap)
{
	_r1 = r1;
	_r2 = r2;
	_normal = normal;
	_overlap = overlap;
}

float RRCollisionConstraint::b(float dt, float d)
{
	float c1 = 4.0f / (dt * (1 + 4 * d));
	float c2 = 1 / (1 + 4 * d);

	return -c1*_overlap + c2*Gv();
}

float RRCollisionConstraint::Gv()
{
	return glm::dot(_normal, _r1->getVelocity()) + glm::dot(-_normal, _r2->getVelocity());
}

float RRCollisionConstraint::Dii()
{
	return _r1->getInverseMass()*glm::dot(_normal, _normal) + _r2->getInverseMass()*glm::dot(-_normal, -_normal);
}

void RRCollisionConstraint::updateVelocity(float deltaLambda)
{
	glm::vec3 currentVelocity1 = _r1->getVelocity();
	glm::vec3 currentVelocity2 = _r2->getVelocity();

	currentVelocity1 += _r1->getInverseMass()*_normal*deltaLambda;
	currentVelocity2 += _r2->getInverseMass()*-_normal*deltaLambda;

	_r1->setVelocity(currentVelocity1);
	_r2->setVelocity(currentVelocity2);
}

bool RRCollisionConstraint::complementaryCondition()
{
	return true;
}