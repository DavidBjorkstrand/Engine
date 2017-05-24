#pragma once

#include <map>
#include <vector>

#include <glm/gtc/type_ptr.hpp>
#include <GL/glew.h>

struct Particle;
class SphereCollider;

using namespace std;

class SpatialHashing
{
	private:
		const long NUM_BUCKETS = 104729;
		map<long, vector<Particle *> *> *_particleMap;
		map<long, vector<SphereCollider *> *> *_sphereMap;
		vector<GLuint> *_subdivisions;
		vector<Particle *> *_possibleParticleCollisions;
		vector<SphereCollider *> *_possibleSphereCollisions;

	public:
		SpatialHashing();
		void insert(SphereCollider *sphereCollider);
		vector<SphereCollider *> *checkSphere(SphereCollider *sphereCollider);
		vector<SphereCollider *> *checkSphere(Particle *particle);
		void clear();

	private:
		long hash(float x, float y, float z, GLuint subdivision);
		void insertIntoMap(long hash, Particle *particle);
		void insertIntoMap(long hash, SphereCollider *sphereCollider);

};