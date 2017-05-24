#pragma once

#include <vector>

#include <glm/gtc/type_ptr.hpp>
#include <GL/glew.h>

struct Particle;
class Rigidbody;

using namespace std;

struct PPDistanceConstraint {
	Particle *i;
	Particle *j;
	float length;
};

struct RRCollisionConstraint {
	Rigidbody *i;
	Rigidbody *j;
	float overlap;
	glm::vec3 normal;
	GLboolean twoBodies;
};

class ConstraintSolver
{
	private:
		float _dt;
		float _d;
		float _k;
		float _a;
		float _b;
		float _e;

		vector<float> _lambda;
		vector<float> _r;
		vector<float> _D;
		vector<float> _q;

	public:
		ConstraintSolver(float dt);
		void solveConstraints(vector<PPDistanceConstraint> *distanceConstraints);
		void solveConstraints(vector<RRCollisionConstraint> *collisionConstraints);
	
	private:
		float Dkk(PPDistanceConstraint distanceConstraint);
		float qk(PPDistanceConstraint distanceConstraint);
		float Gv(PPDistanceConstraint distanceConstraint);
		float Dkk(RRCollisionConstraint collisionConstraint);
		float qk(RRCollisionConstraint collisionConstraint);
		float Gv(RRCollisionConstraint collisionConstraint);
		float residualNorm2(vector<float> residual, const GLuint NUM_CONSTRAINTS);
};