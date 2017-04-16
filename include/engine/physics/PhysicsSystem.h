#pragma once

#include <vector>

#include <glm/gtc/type_ptr.hpp>
#include <GL/glew.h>

class Scene;
class Collider;

struct Particle
{
	GLuint index;
	float radius;
	float mass;
	float inverseMass;
	glm::vec3 position;
	glm::vec3 velocity;
	glm::vec3 predictedVelocity;
	glm::vec3 acceleration;
	glm::vec3 force;
};

using namespace std;

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
		void applyExternalForces(vector<vector<Particle> *> *particles);
		void applyGravity(Particle *particle);
		void applyViscousFriction(Particle *particle);
		void applyAirFriction(Particle *particle);
		void collisionResolution(vector<vector<Particle> *> *particles, vector<Collider *> *colliders);
		void integrate(vector<vector<Particle> *> *particles);
		
};