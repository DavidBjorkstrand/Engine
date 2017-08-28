#include "engine/physics/Rigidbody.h"

#include <glm/gtc/type_ptr.hpp>

Rigidbody::Rigidbody(glm::vec3 position, float mass, glm::mat3 I)
{
	_position = position;
	_rotation = glm::quat();
	_velocity = glm::vec3(0.0f);
	_predictedVelocity = glm::vec3(0.0f);
	_angularVelocity = glm::vec3(0.0f);
	_acceleration = glm::vec3(0.0f);
	_force = glm::vec3(0.0f);
	_linearJ = glm::vec3(0.0f);
	_angularJ = glm::vec3(0.0f);
	_mass = mass;
	_inverseMass = 1.0f / mass;
	_I = I;
	_invI = glm::inverse(_I);

	_listeners = new vector<RigidbodyListener *>();
}

void Rigidbody::addListener(RigidbodyListener *rigidbodyListener)
{
	_listeners->push_back(rigidbodyListener);
}

void Rigidbody::setPosition(glm::vec3 position)
{
	_position = position;

	for (vector<RigidbodyListener *>::iterator it = _listeners->begin(); it != _listeners->end(); it++)
	{
		(*it)->notify(_position);
	}
}

void Rigidbody::setRotation(glm::quat rotation)
{
	_rotation = rotation;

	for (vector<RigidbodyListener *>::iterator it = _listeners->begin(); it != _listeners->end(); it++)
	{
		(*it)->notify(_rotation);
	}
}

void Rigidbody::setVelocity(glm::vec3 velocity)
{
	_velocity = velocity;
}

void Rigidbody::setPredictedVelocity(glm::vec3 predictedVelocity)
{
	_predictedVelocity = predictedVelocity;
}

void Rigidbody::setAngularVelocity(glm::vec3 angularVelocity)
{
	_angularVelocity = angularVelocity;
}

void Rigidbody::setAcceleration(glm::vec3 acceleration)
{
	_acceleration = acceleration;
}

void Rigidbody::addForce(glm::vec3 deltaForce)
{
	_force += deltaForce;
}

void Rigidbody::resetForce()
{
	_force = glm::vec3(0.0f);
}

glm::vec3 Rigidbody::getPosition()
{
	return _position;
}

glm::quat Rigidbody::getRotation()
{
	return _rotation;
}

glm::vec3 Rigidbody::getVelocity()
{
	return _velocity;
}

glm::vec3 Rigidbody::getPredictedVelocity()
{
	return _predictedVelocity;
}

glm::vec3 Rigidbody::getAngularVelocity()
{
	return _angularVelocity;
}

glm::vec3 Rigidbody::getAcceleration()
{
	return _acceleration;
}

glm::vec3 Rigidbody::getForce()
{
	return _force;
}

float Rigidbody::getMass()
{
	return _mass;
}

float Rigidbody::getInverseMass()
{
	return _inverseMass;
}

glm::mat3 Rigidbody::getI()
{
	return _I;
}

glm::mat3 Rigidbody::getInvI()
{
	return _invI;
}

void Rigidbody::addLinearImpulse(glm::vec3 impulse)
{
	_linearJ += impulse;
}

void Rigidbody::addAngularImpulse(glm::vec3 impulse)
{
	_angularJ += impulse;
}

void Rigidbody::addImpuleseToVelocity()
{
	_velocity += _inverseMass*_linearJ;
	_angularVelocity += _invI*_angularJ;
}

void Rigidbody::resetImpulse()
{
	_linearJ = glm::vec3(0.0f);
	_angularJ = glm::vec3(0.0f);
}