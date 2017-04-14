#pragma once

#include <vector>

#include <glm/gtc/type_ptr.hpp>
#include <GL/glew.h>

class Scene;

struct Particle
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

class PhysicsSystem
{
	private:
		Scene *_scene;
		float _timeStep;
		float _dtRest;

		// Gravity constant
		glm::vec3 _g;

		// Viscous friction constant
		float _k;

		// Air friction constant
		float _c;

	public:
		PhysicsSystem();
		~PhysicsSystem();
		void setScene(Scene *scene);
		void update(float dt);

	private:
		void applyGravity(Particle *particle);
		void applyViscousFriction(Particle *particle);
		void applyAirFriction(Particle *particle);
};