#pragma once

#include <vector>

#include <glm/gtc/type_ptr.hpp>
#include <GL/glew.h>

class Scene;

struct Rigidbody
{
	GLuint index;
	float mass;
	float inverseMass;
	glm::vec3 position;
	glm::vec3 velocity;
	glm::vec3 predictedVelocity;
	glm::vec3 acceleration;
	glm::vec3 force;
};

struct Interaction
{
	GLuint ri;
	GLuint rj;
	glm::vec3 rij;
	glm::vec3 rvij;
	float drij;
};

class PhysicsSystem
{
	public: 
		void setScene(Scene *scene);
		void update(float dt);

		static Rigidbody *createRigidbody();

	private:
		Scene *_scene;

		static Rigidbody _rigidbodies[20000];
		static GLuint _idCounter;
};