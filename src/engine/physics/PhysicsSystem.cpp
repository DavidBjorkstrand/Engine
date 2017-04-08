#include "engine/physics/PhysicsSystem.h"

#include "engine/scene/Scene.h"

#include <vector>
#include <iostream>
#include <string>
#include <map>

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>


Rigidbody PhysicsSystem::_rigidbodies[20000];
GLuint PhysicsSystem::_idCounter = 0;

void PhysicsSystem::setScene(Scene *scene)
{
	_scene = scene;
}

void PhysicsSystem::update(float dt)
{

	if (dt > 1.0f / 60.0f)
	{
		dt = 1.0f / 60.0f;
	}

	_scene->traverse();
	vector<Rigidbody *> *rigidbodies = _scene->getRigidbodies();

	for (Rigidbody *rigbody : *rigidbodies)
	{
		glm::vec3 sphere = glm::vec3(0.0f, 6.0f, -6.0f);
		glm::vec3 n = rigbody->position - sphere;

		if (glm::length(n) < 1.5f)
		{
			glm::vec3 v = rigbody->velocity;
			glm::vec3 pv = rigbody->predictedVelocity;

			rigbody->velocity = v - (1.0f + 0.0f)*glm::dot(v, n)*n;
			rigbody->position += 0.05f * n;
		}
	}

	for (Rigidbody *rigbody : *rigidbodies)
	{
		glm::vec3 n = glm::vec3(0.0f, 1.0f, 0.0f);

		if (rigbody->position.y <= 0.2f)
		{
			glm::vec3 v = rigbody->velocity;
			glm::vec3 pv = rigbody->predictedVelocity;

			rigbody->velocity = v - (1.0f + 0.0f)*glm::dot(v, n)*n;
			rigbody->position += 0.05f * n;
		}

		/*n = glm::vec3(-1.0f, 0.0f, 0.0f);
		if (rigbody->position.x >= 2.0f)
		{
			glm::vec3 v = rigbody->velocity;
			glm::vec3 pv = rigbody->predictedVelocity;

			rigbody->velocity = v - (1.0f + 0.0f)*glm::dot(v, n)*n;
			rigbody->position += 0.05f * n;
		}

		n = glm::vec3(1.0f, 0.0f, 0.0f);
		if (rigbody->position.x <= -2.0f)
		{
			glm::vec3 v = rigbody->velocity;
			glm::vec3 pv = rigbody->predictedVelocity;

			rigbody->velocity = v - (1.0f + 0.0f)*glm::dot(v, n)*n;
			rigbody->position += 0.05f * n;
		}

		n = glm::vec3(0.0f, 0.0f, 1.0f);
		if (rigbody->position.z <= -15.0f)
		{
			glm::vec3 v = rigbody->velocity;
			glm::vec3 pv = rigbody->predictedVelocity;

			rigbody->velocity = v - (1.0f + 0.0f)*glm::dot(v, n)*n;
			rigbody->position += 0.05f * n;
		}
		n = glm::vec3(0.0f, 0.0f, -1.0f);
		if (rigbody->position.z >= 1.0f)
		{
			glm::vec3 v = rigbody->velocity;
			glm::vec3 pv = rigbody->predictedVelocity;

			rigbody->velocity = v - (1.0f + 0.0f)*glm::dot(v, n)*n;
			rigbody->position += 0.05f * n;
		}*/
	}

	for (Rigidbody *rigidbody : *rigidbodies)
	{
		rigidbody->position = rigidbody->position + rigidbody->velocity*dt + 0.5f*rigidbody->acceleration*dt*dt;
		rigidbody->predictedVelocity = rigidbody->velocity + rigidbody->acceleration*dt;
	}


	/*float L = 0.3f;
	float kS = 50.0f;
	float kD = 1;

	vector<Interaction> interactions;

	for (GLuint i = 0; i < rigidbodies->size(); i++)
	{
		for (GLuint j = i+1; j < rigidbodies->size(); j++)
		{
			Rigidbody *ri = (*rigidbodies)[i];
			Rigidbody *rj = (*rigidbodies)[j];

			glm::vec3 rij = ri->position - rj->position;
			float drij = glm::length(rij);

			if (drij < L)
			{
				Interaction interaction;

				interaction.ri = ri->index;
				interaction.rj = rj->index;
				interaction.drij = drij;
				interaction.rij = rij;
				interaction.rvij = ri->predictedVelocity - rj->predictedVelocity;

				interactions.push_back(interaction);
			}
		}
	}

	

	for (Interaction interaction : interactions)
	{
		float lhs = kS*(interaction.drij - L);
		float damping = kD*((glm::dot(interaction.rvij, interaction.rij)) / interaction.drij);
		glm::vec3 force = -(lhs + damping)*glm::normalize(interaction.rij);

		PhysicsSystem::_rigidbodies[interaction.ri].force += force;
		PhysicsSystem::_rigidbodies[interaction.rj].force +- force;
	}*/


	for (Rigidbody *rigidbody : *rigidbodies)
	{
		glm::vec3 uHat = glm::normalize(rigidbody->predictedVelocity);
		float u = glm::length(rigidbody->predictedVelocity);
		rigidbody->force += glm::vec3(0.0f, -rigidbody->mass * 9.82f, 0.0f);
		rigidbody->force -= 0.01f * u*uHat;
		rigidbody->force -= 0.01f * u*u*uHat;
	}

	for (Rigidbody *rigidbody : *rigidbodies)
	{
		glm::vec3 acceleration = rigidbody->force * rigidbody->inverseMass;
		glm::vec3 velocity = rigidbody->velocity + 0.5f*(rigidbody->acceleration + acceleration)*dt;

		rigidbody->force = glm::vec3(0.0);
		rigidbody->velocity = velocity;
		rigidbody->acceleration = acceleration;
	}
}

Rigidbody *PhysicsSystem::createRigidbody()
{
	Rigidbody rigidbody;
	Rigidbody *returnValue;

	rigidbody.index = _idCounter;
	rigidbody.mass = 0.1f;
	rigidbody.inverseMass = 1.0f / 0.1f;
	rigidbody.position = glm::vec3(0.0f);
	rigidbody.velocity = glm::vec3(0.0f);
	rigidbody.acceleration = glm::vec3(0.0f);
	rigidbody.force = glm::vec3(0.0f);

	_rigidbodies[_idCounter] = rigidbody;

	returnValue = &_rigidbodies[_idCounter];

	_idCounter++;

	return returnValue;
}