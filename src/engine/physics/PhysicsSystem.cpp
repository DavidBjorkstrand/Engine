#include "engine/physics/PhysicsSystem.h"

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
		for (vector<Particle> *particleVector : *particles)
		{
			for (GLuint i = 0; i < particleVector->size(); i++)
			{
				Particle particle = particleVector->at(i);
				//r_(n+1)
				(*particleVector)[i].position = particle.position + particle.velocity*_timeStep + 0.5f*particle.acceleration*_timeStep*_timeStep;
				(*particleVector)[i].predictedVelocity = particle.velocity + particle.acceleration*_timeStep;

				applyGravity(&(*particleVector)[i]);
				applyViscousFriction(&(*particleVector)[i]);
				applyAirFriction(&(*particleVector)[i]);

				//a_(n+1)
				glm::vec3 newAcceleration = (*particleVector)[i].force * particle.inverseMass;

				//v_(n+1)
				(*particleVector)[i].velocity = particle.velocity + 0.5f*(particle.acceleration + newAcceleration)*_timeStep;

				(*particleVector)[i].acceleration = newAcceleration;
				(*particleVector)[i].force = glm::vec3(0.0f);
			}
		}

		_dtRest -= _timeStep;
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