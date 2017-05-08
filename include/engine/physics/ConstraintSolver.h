#pragma once

#include <vector>

#include <GL/glew.h>

struct Particle;

using namespace std;

struct DistanceConstraint {
	Particle *i;
	Particle *j;
	float length;
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

	public:
		ConstraintSolver(float dt);
		void solveConstraints(vector<DistanceConstraint> *distanceConstraints);
	
	private:
		float S(GLuint i, GLuint j, vector<DistanceConstraint> *distanceConstraints);
		float B(DistanceConstraint distanceConstraint);
};