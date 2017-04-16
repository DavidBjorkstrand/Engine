#include "engine/physics/PhysicsSystem.h"

#include "engine/physics/Collider.h"
#include "engine/scene/Scene.h"

#include <vector>
#include <iostream>
#include <string>
#include <map>

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

PhysicsSystem::PhysicsSystem()
{
	_timeStep = 1.0f / 60.0f;
	_dtRest = 0.0f;

	_g = glm::vec3(0.0f, -9.82f, 0.0f);
	_k = 0.01f;
	_c = 0.01f;
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
		vector<vector<Particle>*> *particles = _scene->getParticles();
		vector<Collider *> *colliders = _scene->getColliders();

		applyExternalForces(particles);
		collisionResolution(particles, colliders);
		integrate(particles);

		_dtRest -= _timeStep;
	}
}

void PhysicsSystem::applyExternalForces(vector<vector<Particle> *> *particles)
{
	for (vector<Particle> *particleVector : *particles)
	{
		for (GLuint i = 0; i < particleVector->size(); i++)
		{
			applyGravity(&(*particleVector)[i]);
			applyViscousFriction(&(*particleVector)[i]);
			applyAirFriction(&(*particleVector)[i]);
		}
	}
}

void PhysicsSystem::applyGravity(Particle *particle)
{
	particle->force += _g * particle->mass;
}

void PhysicsSystem::applyViscousFriction(Particle *particle)
{
	glm::vec3 uHat = glm::normalize(particle->predictedVelocity);
	float u = glm::length(particle->predictedVelocity);

	particle->force += -_k*u*uHat;
}

void PhysicsSystem::applyAirFriction(Particle *particle)
{
	glm::vec3 uHat = glm::normalize(particle->predictedVelocity);
	float u = glm::length(particle->predictedVelocity);

	particle->force += -_c*u*u*uHat;
}

void PhysicsSystem::collisionResolution(vector<vector<Particle> *> *particles, vector<Collider *> *colliders)
{
	for (vector<Particle> *particleVector : *particles)
	{
		for (GLuint i = 0; i < particleVector->size(); i++)
		{
			Particle particle = particleVector->at(i);
			for (Collider *collider : *colliders)
			{
				Collision collision = collider->checkCollision(&(*particleVector)[i]);

				if (collision.colliding)
				{
					//cout << glm::abs(glm::dot(-collision.normal, particle.velocity)) << endl;
					glm::vec3 normalVelocity = glm::dot(-collision.normal, particle.velocity)*-collision.normal;
					float NdotV = glm::dot(-collision.normal, particle.velocity);
					//cout << NdotV << endl;

					//cout << glm::to_string(normalVelocity) << endl;
					if (NdotV < 0.0f && glm::length(normalVelocity) > 0.5f)
					{
						glm::vec3 newVelocity = particle.velocity - (1.0f + 0.4f)*normalVelocity;
						(*particleVector)[i].velocity = newVelocity;
						//cout << "Impulse: " << glm::to_string(normalVelocity) << endl;
					}
					else if (NdotV < 0.00001f)
					{
						float kS = 10.0f;
						float d = 5.0f;
						float e = (4.0f / kS) / ((_timeStep*_timeStep)*(1.0f + 4.0f*d));
						float a = (4.0f / _timeStep) / (1.0f + 4.0f*d);
						float b = (4.0f*d) / (1.0f + 4.0f*d);
						float q = collision.distance;
						glm::vec3 v = normalVelocity;
						glm::vec3 f_m = glm::dot(-collision.normal, (particle.force*particle.inverseMass))*-collision.normal;

						glm::vec3 lambda = (-(a / _timeStep)*q*-collision.normal - b*v -_timeStep*f_m) / (1.0f + e*particle.mass);
						
						//cout << "SPOOK: " << glm::to_string(lambda) << endl;

						(*particleVector)[i].velocity -= lambda;
					}
				}
			}
		}
	}
}

void PhysicsSystem::integrate(vector<vector<Particle> *> *particles)
{
	for (vector<Particle> *particleVector : *particles)
	{
		for (GLuint i = 0; i < particleVector->size(); i++)
		{
			Particle particle = particleVector->at(i);

			//a_(n)
			glm::vec3 newAcceleration = (*particleVector)[i].force * particle.inverseMass;

			//v_(n)
			(*particleVector)[i].velocity = particle.velocity + 0.5f*(particle.acceleration + newAcceleration)*_timeStep;

			(*particleVector)[i].acceleration = newAcceleration;
			(*particleVector)[i].force = glm::vec3(0.0f);

			//r_(n+1)
			(*particleVector)[i].position = particle.position + particle.velocity*_timeStep + 0.5f*particle.acceleration*_timeStep*_timeStep;
			(*particleVector)[i].predictedVelocity = particle.velocity + particle.acceleration*_timeStep;
		}
	}
}

