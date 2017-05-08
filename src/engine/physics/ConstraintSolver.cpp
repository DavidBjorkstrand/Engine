#include "engine/physics/ConstraintSolver.h"
#include "engine/physics/ParticleSystem.h"

#include <vector>
#include <iostream>

#include <glm/gtc/type_ptr.hpp>
#include <GL/glew.h>

ConstraintSolver::ConstraintSolver(float dt)
{
	_dt = dt;
	_d = 3.0f;
	_k = 100000.0f;
	_a = 4.0f / (dt * (1 + 4 * _d));
	_b = (4.0f*_d) / (1 + 4 * _d);
	_e = 4.0f / (dt * dt * _k * (1 + 4 * _d));
}

void ConstraintSolver::solveConstraints(vector<DistanceConstraint> *distanceConstraints)
{
	const GLuint MAX_IT = 10;
	
	Particle start;
	start.index = 1000;
	start.mass = 0.1f;
	start.inverseMass = 10.0f;
	start.position = glm::vec3(-1.0f, 40.0f, 0.0f);
	start.acceleration = glm::vec3(0.0f);
	start.force = glm::vec3(0.0f);
	start.velocity = glm::vec3(0.0f);

	DistanceConstraint dc;
	dc.i = &start;
	dc.j = distanceConstraints->at(0).i;
	dc.length = 1.0f;
	distanceConstraints->insert(distanceConstraints->begin(), dc);
	
	GLuint numConstraints = distanceConstraints->size();
	float *lambda = new float[numConstraints];

	for (GLuint i = 0; i < numConstraints; i++)
	{
		lambda[i] = 0.0f;
	}

	for (GLuint it = 0; it < MAX_IT; it++)
	{
		for (GLuint i = 0; i < numConstraints; i++)
		{

			DistanceConstraint distanceConstraint = distanceConstraints->at(i);
 			float rhs = B(distanceConstraint);

			for (GLuint j = 0; j < numConstraints; j++)
			{
				if (i != j)
				{
					rhs -= S(i, j, distanceConstraints)*lambda[j];
				}
			}

			lambda[i] = (1 / (S(i, i, distanceConstraints)))*rhs;

		}

	}

	for (GLuint i = 0; i < numConstraints; i++)
	{
		DistanceConstraint distanceConstraint = distanceConstraints->at(i);
		glm::vec3 jac_i = glm::normalize(distanceConstraint.i->position - distanceConstraint.j->position);
		glm::vec3 jac_j = -jac_i;

		distanceConstraint.i->velocity += lambda[i] * jac_i * distanceConstraint.i->inverseMass;
		distanceConstraint.j->velocity += lambda[i] * jac_j * distanceConstraint.i->inverseMass;
	}

	distanceConstraints->erase(distanceConstraints->begin());
	delete lambda;

}

float ConstraintSolver::S(GLuint i, GLuint j, vector<DistanceConstraint> *distanceConstraints)
{
	DistanceConstraint constraint_i = distanceConstraints->at(i);
	DistanceConstraint constraint_j = distanceConstraints->at(j);
	float Sij = 0.0f;

	if (constraint_i.i->index == constraint_j.i->index)
	{
		glm::vec3 jac_i = glm::normalize(constraint_i.i->position - constraint_i.j->position);
		glm::vec3 jac_j = glm::normalize(constraint_j.i->position - constraint_j.j->position);

		Sij += constraint_i.i->inverseMass * glm::dot(jac_i, jac_j);
	}

	if (constraint_i.i->index == constraint_j.j->index)
	{
		glm::vec3 jac_i = glm::normalize(constraint_i.i->position - constraint_i.j->position);
		glm::vec3 jac_j = glm::normalize(constraint_j.j->position - constraint_j.i->position);

		Sij += constraint_i.i->inverseMass * glm::dot(jac_i, jac_j);
	}

	if (constraint_i.j->index == constraint_j.j->index)
	{
		glm::vec3 jac_i = glm::normalize(constraint_i.j->position - constraint_i.i->position);
		glm::vec3 jac_j = glm::normalize(constraint_j.j->position - constraint_j.i->position);

		Sij += constraint_i.j->inverseMass * glm::dot(jac_i, jac_j);
	}

	if (i == j)
	{
		Sij += _e;
	}

	return Sij;
}

float ConstraintSolver::B(DistanceConstraint distanceConstraint)
{
	float Bi = 0.0f;

	glm::vec3 jac_i = glm::normalize(distanceConstraint.i->position - distanceConstraint.j->position);
	glm::vec3 jac_j = -jac_i;

	// -dt*G*inv(M)*f
	Bi -= _dt*glm::dot(jac_i, distanceConstraint.i->acceleration) + glm::dot(jac_j, distanceConstraint.j->acceleration);

	// - b*G*v
	Bi -= _b*glm::dot(jac_i, distanceConstraint.i->velocity) + glm::dot(jac_j, distanceConstraint.j->velocity);

	// -a * q
	Bi -= _a*(glm::length(distanceConstraint.i->position - distanceConstraint.j->position) - distanceConstraint.length);

	return Bi;
}