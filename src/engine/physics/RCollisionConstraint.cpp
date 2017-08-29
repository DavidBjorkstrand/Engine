#include "engine/physics/RCollisionConstraint.h"

#include "engine/physics/Rigidbody.h"
#include "engine/physics/Collider.h"

#include <glm/gtc/type_ptr.hpp>

RCollisionConstraint::RCollisionConstraint(Rigidbody *rigidbody, glm::vec3 normal, float overlap)
{
	_rigidbody = rigidbody;
	_normal = normal;
	_overlap = overlap;
}

float RCollisionConstraint::b(float dt, float d)
{
	float c1 = 4.0f / (dt * (1 + 4 * d));
	float c2 = 1 / (1 + 4 * d);

	return -c1*_overlap + c2*Gv();
}

float RCollisionConstraint::Gv()
{
	return glm::dot(_normal, _rigidbody->getVelocity());
}

float RCollisionConstraint::Dii()
{
	return _rigidbody->getInverseMass()*glm::dot(_normal, _normal);
}

void RCollisionConstraint::updateVelocity(float deltaLambda)
{
	glm::vec3 currentVelocity = _rigidbody->getVelocity();

	currentVelocity += _rigidbody->getInverseMass()*_normal*deltaLambda;

	_rigidbody->setVelocity(currentVelocity);
}

bool RCollisionConstraint::complementaryCondition()
{
	return true;
}