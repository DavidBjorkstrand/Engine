#include "engine/physics/PhysicsSystem.h"

#include "engine/physics/ParticleSystem.h"
#include "engine/physics/ConstraintSolver.h"
#include "engine/physics/Collider.h"
#include "engine/physics/SoftBody.h"
#include "engine/scene/Scene.h"

#include <vector>
#include <iostream>
#include <string>
#include <map>

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

PhysicsSystem::PhysicsSystem()
{
	_timeStep = 1.0f / 500.0f;
	_dtRest = 0.0f;

	_constraintSolver = new ConstraintSolver(_timeStep);

	_g = glm::vec3(0.0f, -9.82f, 0.0f);
	_k = 0.0005f;
	_c = 0.0005f;
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

	while (_dtRest >= _timeStep)
	{
		vector<ParticleSystem *> *particleSystems = _scene->getParticleSystems();
		vector<Collider *> *colliders = _scene->getColliders();
		vector<SoftBody *> *softBodies = _scene->getSoftBodies();

		applyConstraints(softBodies);
		applyExternalForces(particleSystems);
		applyExternalForces(softBodies);
		collisionResolution(particleSystems, colliders);
		integrate(particleSystems);
		integrate(softBodies);

		_dtRest -= _timeStep;
	}
}

void PhysicsSystem::applyConstraints(vector<SoftBody *> *softBodies)
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

void PhysicsSystem::applyGravity(Particle *particle)
{
	particle->force += _g * particle->mass;
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

void PhysicsSystem::applyAirFriction(Particle *particle)
{
	if (particle->predictedVelocity != glm::vec3(0.0f, 0.0f, 0.0f))
	{
		glm::vec3 uHat = glm::normalize(particle->predictedVelocity);
		float u = glm::length(particle->predictedVelocity);

		particle->force += -_c*u*u*uHat;
	}
}

void PhysicsSystem::collisionResolution(vector<ParticleSystem *> *particleSystems, vector<Collider *> *colliders)
{
	for (ParticleSystem *particleSystem : *particleSystems)
	{
		for (ParticleSystem::iterator it = particleSystem->begin(); it != particleSystem->end(); it++)
		{
			Particle *particle = &it;
			for (Collider *collider : *colliders)
			{
				Collision collision = collider->checkCollision(particle);

				if (collision.colliding)
				{
					glm::vec3 normalVelocity = glm::dot(-collision.normal, particle->velocity)*-collision.normal;
					float NdotV = glm::dot(-collision.normal, particle->velocity);

					if (NdotV < 0.0f && glm::length(normalVelocity) > 0.5f)
					{
						glm::vec3 newVelocity = particle->velocity - (1.0f + 0.1f)*normalVelocity;
						particle->velocity = newVelocity;
					}
					else if (NdotV < 0.00001f)
					{
						float kS = 1000.0f;
						float d = 5.0f;
						float e = (4.0f / kS) / ((_timeStep*_timeStep)*(1.0f + 4.0f*d));
						float a = (4.0f / _timeStep) / (1.0f + 4.0f*d);
						float b = (4.0f*d) / (1.0f + 4.0f*d);
						float q = collision.distance;
						glm::vec3 v = normalVelocity;
						glm::vec3 f_m = glm::dot(-collision.normal, (particle->force*particle->inverseMass))*-collision.normal;

						glm::vec3 lambda = (-(a / _timeStep)*q*-collision.normal - b*v -_timeStep*f_m) / (1.0f + e*particle->mass);

						particle->force -= lambda;
					}
				}
			}
		}
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
			particle->velocity = particle->velocity + 0.5f*(particle->acceleration + newAcceleration)*_timeStep;

			particle->acceleration = newAcceleration;
			particle->force = glm::vec3(0.0f);

			//r_(n+1)
			particle->position = particle->position + particle->velocity*_timeStep + 0.5f*particle->acceleration*_timeStep*_timeStep;
			particle->predictedVelocity = particle->velocity + particle->acceleration*_timeStep;
		}
	}
}

void PhysicsSystem::integrate(vector<SoftBody *> *softBodies)
{
	for (SoftBody *softBody : *softBodies)
	{
		for (ParticleSystem::iterator it = softBody->begin(); it != softBody->end(); it++)
		{
			Particle *particle = &it;

			//a_(n)
			glm::vec3 newAcceleration = particle->force * particle->inverseMass;

			//v_(n)
			particle->velocity = particle->velocity + 0.5f*(particle->acceleration + newAcceleration)*_timeStep;

			particle->acceleration = newAcceleration;
			particle->force = glm::vec3(0.0f);

		}

		if (!softBody->getDistanceConstraints()->empty())
		{
			_constraintSolver->solveConstraints(softBody->getDistanceConstraints());
		}

		for (ParticleSystem::iterator it = softBody->begin(); it != softBody->end(); it++)
		{
			Particle *particle = &it;

			//r_(n+1)
			particle->position = particle->position + particle->velocity*_timeStep + 0.5f*particle->acceleration*_timeStep*_timeStep;
			particle->predictedVelocity = particle->velocity + particle->acceleration*_timeStep;
		}
	}
}

