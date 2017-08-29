#pragma once

#include <vector>

#include <glm/gtc/type_ptr.hpp>
#include <GL/glew.h>

class Scene;
class Collider;
class SoftBody;
class ParticleSystem;
class ConstraintSolver;
class SpatialHashing;
class Rigidbody;
class PlaneCollider;

struct Particle;

using namespace std;

struct SpringConstraint
{
	Particle *i;
	Particle *j;
	float ks;
	float kd;
	float length;
};

class PhysicsSystem
{
	private:
		Scene *_scene;
		ConstraintSolver *_constraintSolver;
		SpatialHashing *_spatialHashing;
		float _dt;
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
		void applySpringConstraints(vector<SoftBody *> *softBodies);
		void applyExternalForces(vector<ParticleSystem *> *particleSystems);
		void applyExternalForces(vector<SoftBody *> *softBodies);
		void applyExternalForces(vector<Rigidbody *> *rigidbodies);
		void applyGravity(Particle *particle);
		void applyGravity(Rigidbody *rigidbody);
		void applyViscousFriction(Particle *particle);
		void applyViscousFriction(Rigidbody *rigidbody);
		void applyAirFriction(Particle *particle);
		void applyAirFriction(Rigidbody *rigidbody);
		void collisionResolution(vector<ParticleSystem *> *particleSystems, vector<SoftBody *> *softbodies, vector<Collider *> *colliders);
		void particleSpherePlane(Particle *particle, vector<PlaneCollider *> planeColliders);
		void integrate(vector<ParticleSystem *> *particleSystems);
		void integrate(vector<SoftBody *> *softBodies);
		void integrate(vector<Rigidbody *> *rigidbodies);
		
};