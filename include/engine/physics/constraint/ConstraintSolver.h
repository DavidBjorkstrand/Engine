#pragma once

#include <vector>

#include <glm/gtc/type_ptr.hpp>
#include <GL/glew.h>

class Constraint;

using namespace std;

class ConstraintSolver
{
	private:
		float _dt;
		float _d;
		float _k;
		float _e;

		vector<Constraint *> _constraints;
		vector<float> _lambda;
		vector<float> _r;
		vector<float> _D;
		vector<float> _q;

	public:
		ConstraintSolver(float dt);
		void addConstraint(Constraint *constraint);
		void solve();

};