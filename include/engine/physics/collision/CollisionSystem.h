#pragma once

#include <vector>

class ConstraintSolver;
class SpatialHashing;
class ParticleSystem;
class SoftBody;
class Collider;
class SphereCollider;
class PlaneCollider;

struct Particle;

using namespace std;

class CollisionSystem
{
	private:
		ConstraintSolver *_constraintSolver;
		SpatialHashing *_spatialHashing;
		vector<SphereCollider *> _sphereColliders;
		vector<PlaneCollider *> _planeColliders;

	public:
		CollisionSystem(ConstraintSolver *constraintSolver);
		void collisionResolution(vector<ParticleSystem *> *particleSystems, vector<SoftBody *> *softbodies, vector<Collider *> *colliders);

	private:
		void extractColliders(vector<Collider *> *colliders);
		void setupSpatialHashing();
		void particleSphereResolution(Particle *particle);
		void particlePlaneResolution(Particle *particle);
		void sphereSphereResolution(SphereCollider *collider);
		void spherePlaneResolution(SphereCollider *collider);
};