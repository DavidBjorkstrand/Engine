#include "engine/physics/PhysicsSystem.h"

#include "engine/physics/ParticleSystem.h"
#include "engine/physics/constraint/ConstraintSolver.h"
#include "engine/physics/collision/CollisionSystem.h"
#include "engine/physics/collision/SpatialHashing.h"
#include "engine/physics/collision/Collider.h"
#include "engine/physics/SoftBody.h"
#include "engine/physics/Rigidbody.h"
#include "engine/physics/collision/SphereCollider.h"
#include "engine/physics/collision/PlaneCollider.h"
#include "engine/scene/Scene.h"

#include <vector>
#include <iostream>
#include <string>
#include <map>
#include <chrono>
#include <fstream>

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/matrix_cross_product.hpp>

PhysicsSystem::PhysicsSystem()
{
	_dt = 1.0f / 500.0f;
	_dtRest = 0.0f;

	_constraintSolver = new ConstraintSolver(_dt);
	_collisionSystem = new CollisionSystem(_constraintSolver);

	_g = glm::vec3(0.0f, -9.82f, 0.0f);
	_k = 0.00005f;
	_c = 0.00005f;
}

PhysicsSystem::~PhysicsSystem()
{

}

void PhysicsSystem::setScene(Scene *scene)
{
	_scene = scene;
}

void PhysicsSystem::update(float dt)
{
	_scene->traverse();
	_dtRest += dt;
	while (_dtRest >= _dt)
	{
		vector<ParticleSystem *> *particleSystems = _scene->getParticleSystems();
		vector<Collider *> *colliders = _scene->getColliders();
		vector<SoftBody *> *softBodies = _scene->getSoftBodies();
		vector<Rigidbody *> *rigidbodies = _scene->getRigidbodies();

		applySpringConstraints(softBodies);
		applyExternalForces(particleSystems);
		applyExternalForces(softBodies);
		applyExternalForces(rigidbodies);

		_collisionSystem->collisionResolution(particleSystems, softBodies, colliders);

		for (SoftBody *softbody : *softBodies)
		{
			for (Constraint *constraint : *softbody->getConstraints())
			{
				_constraintSolver->addConstraint(constraint);
			}
		}
		_constraintSolver->solve();

		integrate(particleSystems);
		integrate(softBodies);
		integrate(rigidbodies);

		_dtRest -= _dt;
	}
}

void PhysicsSystem::applySpringConstraints(vector<SoftBody *> *softBodies)
{
	for (SoftBody *softBody : *softBodies)
	{
		vector<SpringConstraint> *constraints = softBody->getSpringConstraints();
		for (SpringConstraint s : *constraints)
		{
			glm::vec3 r_ij = s.i->position - s.j->position;
			glm::vec3 v_ij = s.i->velocity - s.j->velocity;
			glm::vec3 dir = glm::normalize(r_ij);
			float length = glm::length(r_ij);

			glm::vec3 f_i = -(s.ks*(length - s.length) + s.kd*(glm::dot(v_ij, r_ij) / length))*dir;
			glm::vec3 f_j = -f_i;

			s.i->force += f_i;
			s.j->force += f_j;
		}
	}
}

void PhysicsSystem::applyExternalForces(vector<ParticleSystem *> *particleSystems)
{
	for (ParticleSystem *particleSystem : *particleSystems)
	{
		for (ParticleSystem::iterator it = particleSystem->begin(); it != particleSystem->end(); it++)
		{
			applyGravity(&it);
			applyViscousFriction(&it);
			applyAirFriction(&it);
		}
	}
}

void PhysicsSystem::applyExternalForces(vector<SoftBody *> *softBodies)
{
	for (SoftBody *softBody : *softBodies)
	{
		for (ParticleSystem::iterator it = softBody->begin(); it != softBody->end(); it++)
		{
			applyGravity(&it);
			applyViscousFriction(&it);
			applyAirFriction(&it);
		}
	}
}

void PhysicsSystem::applyExternalForces(vector<Rigidbody *> *rigidbodies)
{
	for (Rigidbody *rigidbody : *rigidbodies)
	{
		applyGravity(rigidbody);
		applyViscousFriction(rigidbody);
		applyAirFriction(rigidbody);
	}
}

void PhysicsSystem::applyGravity(Particle *particle)
{
	particle->force += _g * particle->mass;
}

void PhysicsSystem::applyGravity(Rigidbody *rigidbody)
{
	rigidbody->addForce(_g * rigidbody->getMass());
}

void PhysicsSystem::applyViscousFriction(Particle *particle)
{
	if (particle->predictedVelocity != glm::vec3(0.0f, 0.0f, 0.0f))
	{
		glm::vec3 uHat = glm::normalize(particle->predictedVelocity);
		float u = glm::length(particle->predictedVelocity);

		particle->force += -_k*u*uHat;
	}
}

void PhysicsSystem::applyViscousFriction(Rigidbody *rigidbody)
{
	if (rigidbody->getPredictedVelocity() != glm::vec3(0.0f, 0.0f, 0.0f))
	{
		glm::vec3 uHat = glm::normalize(rigidbody->getPredictedVelocity());
		float u = glm::length(rigidbody->getPredictedVelocity());

		rigidbody->addForce(-_k*u*uHat);
	}
}

void PhysicsSystem::applyAirFriction(Particle *particle)
{
	if (particle->predictedVelocity != glm::vec3(0.0f, 0.0f, 0.0f))
	{
		glm::vec3 uHat = glm::normalize(particle->predictedVelocity);
		float u = glm::length(particle->predictedVelocity);

		particle->force += -_c*u*u*uHat;
	}
}

void PhysicsSystem::applyAirFriction(Rigidbody *rigidbody)
{
	if (rigidbody->getPredictedVelocity() != glm::vec3(0.0f, 0.0f, 0.0f))
	{
		glm::vec3 uHat = glm::normalize(rigidbody->getPredictedVelocity());
		float u = glm::length(rigidbody->getPredictedVelocity());

		rigidbody->addForce(-_c*u*u*uHat);
	}
}

void PhysicsSystem::integrate(vector<ParticleSystem *> *particleSystems)
{
	for (ParticleSystem *particleSystem : *particleSystems)
	{
		for (ParticleSystem::iterator it = particleSystem->begin(); it != particleSystem->end(); it++)
		{
			Particle *particle = &it;

			//a_(n)
			glm::vec3 newAcceleration = particle->force * particle->inverseMass;

			//v_(n)
			particle->velocity = particle->velocity + 0.5f*(particle->acceleration + newAcceleration)*_dt;

			particle->acceleration = newAcceleration;
			particle->force = glm::vec3(0.0f);

			//r_(n+1)
			particle->position = particle->position + particle->velocity*_dt + 0.5f*particle->acceleration*_dt*_dt;
			particle->predictedVelocity = particle->velocity + particle->acceleration*_dt;
		}
	}
}

void PhysicsSystem::integrate(vector<SoftBody *> *softBodies)
{
	for (SoftBody *softBody : *softBodies)
	{
		/*if (!softBody->getDistanceConstraints()->empty())
		{
			_constraintSolver->solveConstraints(softBody->getDistanceConstraints());
		}*/

		for (ParticleSystem::iterator it = softBody->begin(); it != softBody->end(); it++)
		{
			Particle *particle = &it;

			//a_(n)
			glm::vec3 newAcceleration = particle->force * particle->inverseMass;

			//v_(n)
			particle->velocity = particle->velocity + 0.5f*(particle->acceleration + newAcceleration)*_dt;

			particle->acceleration = newAcceleration;
			particle->force = glm::vec3(0.0f);

			//r_(n+1)
			particle->position = particle->position + particle->velocity*_dt + 0.5f*particle->acceleration*_dt*_dt;
			particle->predictedVelocity = particle->velocity + particle->acceleration*_dt;

		}
	}
}

void PhysicsSystem::integrate(vector<Rigidbody *> *rigidbodies)
{
	for (Rigidbody *rigidbody : *rigidbodies)
	{
		glm::vec3 newAcceleration = rigidbody->getForce() * rigidbody->getInverseMass();
		glm::vec3 torqueGyro = glm::cross(rigidbody->getAngularVelocity(), rigidbody->getI()*rigidbody->getAngularVelocity());

		rigidbody->setVelocity(rigidbody->getVelocity() + 0.5f*(rigidbody->getAcceleration() + newAcceleration)*_dt);
		rigidbody->setAngularVelocity(rigidbody->getAngularVelocity() + _dt*rigidbody->getInvI()*torqueGyro);

		rigidbody->setAcceleration(newAcceleration);
		rigidbody->resetForce();

		rigidbody->setPosition(rigidbody->getPosition() + rigidbody->getVelocity()*_dt + 0.5f*rigidbody->getAcceleration()*_dt*_dt);
		rigidbody->setPredictedVelocity(rigidbody->getVelocity() + rigidbody->getAcceleration()*_dt);

		glm::vec3 omega = rigidbody->getAngularVelocity();
		glm::quat q = glm::quat(0.0f, omega)* (_dt / 2.0f);
		q = glm::exp(q);
		q = glm::normalize(q);

		rigidbody->setRotation(glm::normalize(q*rigidbody->getRotation()));

	}
}

