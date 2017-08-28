#include "engine/physics/ConstraintSolver.h"
#include "engine/physics/ParticleSystem.h"
#include "engine/physics/Rigidbody.h"
#include "engine/physics/Constraint.h"

#include <vector>
#include <iostream>
#include <algorithm> 

#include <glm/gtc/type_ptr.hpp>
#include <GL/glew.h>

ConstraintSolver::ConstraintSolver(float dt)
{
	_dt = dt;
	_d = 4.0f;
	_k = 1000000.0f;
	_e = 4.0f / (dt * dt * _k * (1 + 4 * _d));
}

void ConstraintSolver::addConstraint(Constraint *constraint)
{
	_constraints.push_back(constraint);
}

void ConstraintSolver::solve()
{
	float z;
	float deltaLambda;

	for (Constraint *constraint : _constraints)
	{
		_r.push_back(0.0f);
		_lambda.push_back(0.0f);
	}

	for (int it = 0; it < 100; it++)
	{
		int i = 0;
		for (Constraint *constraint : _constraints)
		{
			_r[i] = -constraint->b(_dt, _d) + _e*_lambda[i];
			_r[i] += constraint->Gv();

			z = std::max(0.0f, (-_r[i] / constraint->Dii()) + _lambda[i]);
			deltaLambda = z - _lambda[i];
			_lambda[i] = z;

			constraint->updateVelocity(deltaLambda);

			i++;
		}
	}

	_r.clear();
	_lambda.clear();
	_constraints.clear();
}