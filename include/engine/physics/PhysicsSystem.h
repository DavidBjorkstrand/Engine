#pragma once

#include <vector>

#include <glm/gtc/type_ptr.hpp>
#include <GL/glew.h>

class Scene;
class Collider;
class SpringConstraint;
class ParticleSystem;

struct Particle;

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
		void applyConstraints(vector<vector<SpringConstraint *> *> *springConstraints);
		void applyExternalForces(vector<ParticleSystem *> *particleSystems);
		void applyGravity(Particle *particle);
		void applyViscousFriction(Particle *particle);
		void applyAirFriction(Particle *particle);
		void collisionResolution(vector<ParticleSystem *> *particleSystems, vector<Collider *> *colliders);
		void integrate(vector<ParticleSystem *> *particleSystems);
		
};