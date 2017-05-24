#include "engine/physics/ConstraintSolver.h"
#include "engine/physics/ParticleSystem.h"
#include "engine/physics/Rigidbody.h"

#include <vector>
#include <iostream>

#include <glm/gtc/type_ptr.hpp>
#include <GL/glew.h>

ConstraintSolver::ConstraintSolver(float dt)
{
	_dt = dt;
	_d = 3.0f;
	_a = 4.0f / (dt * (1 + 4 * _d));
	_b = (4.0f*_d) / (1 + 4 * _d);
	_e = 4.0f / (dt * dt * _k * (1 + 4 * _d));
}

void ConstraintSolver::solveConstraints(vector<PPDistanceConstraint> *distanceConstraints)
{
	Particle start;
	start.index = 1000;
	start.mass = 0.1f;
	start.inverseMass = 10.0f;
	start.position = glm::vec3(-1.0f, 40.0f, 0.0f);
	start.acceleration = glm::vec3(0.0f);
	start.force = glm::vec3(0.0f);
	start.velocity = glm::vec3(0.0f);

	PPDistanceConstraint dc;
	dc.i = &start;
	dc.j = distanceConstraints->at(0).i;
	dc.length = 1.0f;
	distanceConstraints->insert(distanceConstraints->begin(), dc);

	_k = 1000.0f;
	_e = 4.0f / (_dt * _dt * _k * (1 + 4 * _d));
	const GLuint MAX_IT = 100;
	const float RESIDUAL_MIN = 0.000001f;
	const GLuint NUM_CONSTRAINTS = distanceConstraints->size();
	float error;

	for (GLuint i = 0; i < NUM_CONSTRAINTS; i++)
	{
		PPDistanceConstraint distanceConstraint = distanceConstraints->at(i);
		_lambda.push_back(0.0f);
		_q.push_back(qk(distanceConstraint));
		_r.push_back(Gv(distanceConstraint) - _q[i]);
		_D.push_back(Dkk(distanceConstraint));
	}
	error = residualNorm2(_r, NUM_CONSTRAINTS);

	GLuint it = 0;

	while (it < MAX_IT && residualNorm2(_r, NUM_CONSTRAINTS) > RESIDUAL_MIN)
	{
		for (GLuint k = 0; k < NUM_CONSTRAINTS; k++)
		{
			PPDistanceConstraint distanceConstraint = distanceConstraints->at(k);
			float z;
			glm::vec3 jac_i = glm::normalize(distanceConstraint.i->position - distanceConstraint.j->position);
			glm::vec3 jac_j = -jac_i;

			_r[k] = -_q[k] + _e*_lambda[k];
			_r[k] += Gv(distanceConstraint);
			error -= residualNorm2(_r, NUM_CONSTRAINTS);

			z = -(1.0f / _D[k])*_r[k];
			_lambda[k] += z;
			distanceConstraint.i->velocity = distanceConstraint.i->velocity + distanceConstraint.i->inverseMass*jac_i*z;
			distanceConstraint.j->velocity = distanceConstraint.j->velocity + distanceConstraint.j->inverseMass*jac_j*z;

			_r[k] = -_q[k] + _e*_lambda[k];
			_r[k] += Gv(distanceConstraint);
			error += residualNorm2(_r, NUM_CONSTRAINTS);

		}
		it++;
	}

	distanceConstraints->erase(distanceConstraints->begin());
	_lambda.clear();
	_r.clear();
	_D.clear();
	_q.clear();
}

void ConstraintSolver::solveConstraints(vector<RRCollisionConstraint> *collisionConstraints)
{
	_k = 1000000.0f;
	_e = 4.0f / (_dt * _dt * _k * (1 + 4 * _d));
	const GLuint MAX_IT = 100;
	const float RESIDUAL_MIN = 0.1f;
	const GLuint NUM_CONSTRAINTS = collisionConstraints->size();
	float error;

	for (GLuint i = 0; i < NUM_CONSTRAINTS; i++)
	{
		RRCollisionConstraint collisionConstraint = collisionConstraints->at(i);
		_lambda.push_back(0.0f);
		_q.push_back(qk(collisionConstraint));
		_r.push_back(Gv(collisionConstraint) - _q[i]);
		_D.push_back(Dkk(collisionConstraint));
	}
	error = residualNorm2(_r, NUM_CONSTRAINTS);

	GLuint it = 0;

	while (it < MAX_IT && residualNorm2(_r, NUM_CONSTRAINTS) > RESIDUAL_MIN)
	{
		for (GLuint k = 0; k < NUM_CONSTRAINTS; k++)
		{
			RRCollisionConstraint collisionConstraint = collisionConstraints->at(k);
			float z;
			glm::vec3 jac_i = collisionConstraint.normal;
			glm::vec3 jac_j = -jac_i;

			_r[k] = -_q[k] + _e*_lambda[k];
			_r[k] += Gv(collisionConstraint);
			error -= residualNorm2(_r, NUM_CONSTRAINTS);

			z = -(1.0f / _D[k])*_r[k];
			_lambda[k] += z;

			if (_lambda[k] < 0.0f)
			{
				z = -(_lambda[k] - z);
				_lambda[k] = 0.0f;
			}

			if (collisionConstraint.twoBodies)
			{
				collisionConstraint.i->setVelocity(collisionConstraint.i->getVelocity() + collisionConstraint.i->getInverseMass()*jac_i*z);
				collisionConstraint.j->setVelocity(collisionConstraint.j->getVelocity() + collisionConstraint.j->getInverseMass()*jac_j*z);
			}
			else
			{
				collisionConstraint.i->setVelocity(collisionConstraint.i->getVelocity() + collisionConstraint.i->getInverseMass()*jac_i*z);
			}
			

			_r[k] = -_q[k] + _e*_lambda[k];
			_r[k] += Gv(collisionConstraint);
			error += residualNorm2(_r, NUM_CONSTRAINTS);

		}
		it++;
	}

	_lambda.clear();
	_r.clear();
	_D.clear();
	_q.clear();
}

float ConstraintSolver::Dkk(PPDistanceConstraint distanceConstraint)
{
	glm::vec3 jac_i = glm::normalize(distanceConstraint.i->position - distanceConstraint.j->position);
	glm::vec3 jac_j = -jac_i;

	return glm::dot(jac_i, jac_i)*distanceConstraint.i->inverseMass + glm::dot(jac_j, jac_j)*distanceConstraint.j->inverseMass + _e;
}

float ConstraintSolver::qk(PPDistanceConstraint distanceConstraint)
{
	float q = 0.0f;
	glm::vec3 jac_i = glm::normalize(distanceConstraint.i->position - distanceConstraint.j->position);
	glm::vec3 jac_j = -jac_i;

	// -dt*G*inv(M)*f
	q -= _dt*(glm::dot(jac_i, distanceConstraint.i->force*distanceConstraint.i->inverseMass) + glm::dot(jac_j, distanceConstraint.j->force*distanceConstraint.j->inverseMass));

	// - b*G*v
	q -= _b*(glm::dot(jac_i, distanceConstraint.i->velocity) + glm::dot(jac_j, distanceConstraint.j->velocity));

	// -a * q
	q -= _a*(glm::length(distanceConstraint.i->position - distanceConstraint.j->position) - distanceConstraint.length);

	return q;
}

float ConstraintSolver::Gv(PPDistanceConstraint distanceConstraint)
{
	glm::vec3 jac_i = glm::normalize(distanceConstraint.i->position - distanceConstraint.j->position);
	glm::vec3 jac_j = -jac_i;

	return glm::dot(distanceConstraint.i->velocity, jac_i) + glm::dot(distanceConstraint.j->velocity, jac_j);
}

float ConstraintSolver::Dkk(RRCollisionConstraint collisionConstraint)
{
	glm::vec3 jac_i = collisionConstraint.normal;
	glm::vec3 jac_j = -jac_i;

	if (collisionConstraint.twoBodies)
	{
		return glm::dot(jac_i, jac_i)*collisionConstraint.i->getInverseMass() + glm::dot(jac_j, jac_j)*collisionConstraint.j->getInverseMass() + _e;
	}
	else
	{
		return glm::dot(jac_i, jac_i)*collisionConstraint.i->getInverseMass() + _e;
	}
}


float ConstraintSolver::qk(RRCollisionConstraint collisionConstraint)
{
	float q = 0.0f;
	glm::vec3 jac_i = collisionConstraint.normal;
	glm::vec3 jac_j = -jac_i;

	// -dt*G*inv(M)*f
	if (collisionConstraint.twoBodies)
	{
		q -= _dt*(glm::dot(jac_i, collisionConstraint.i->getForce()*collisionConstraint.i->getInverseMass()) + glm::dot(jac_j, collisionConstraint.j->getForce()*collisionConstraint.j->getInverseMass()));
	}
	else
	{
		q -= _dt*glm::dot(jac_i, collisionConstraint.i->getForce()*collisionConstraint.i->getInverseMass());
	}
	
	// - b*G*v
	if (collisionConstraint.twoBodies)
	{
		q -= _b*(glm::dot(jac_i, collisionConstraint.i->getVelocity()) + glm::dot(jac_j, collisionConstraint.j->getVelocity()));
	}
	else
	{
		q -= _b*glm::dot(jac_i, collisionConstraint.i->getVelocity());
	}
	
	// -a * q
	q -= _a*collisionConstraint.overlap;

	return q;
}

float ConstraintSolver::Gv(RRCollisionConstraint collisionConstraint)
{
	glm::vec3 jac_i = collisionConstraint.normal;
	glm::vec3 jac_j = -jac_i;

	if (collisionConstraint.twoBodies)
	{
		return glm::dot(collisionConstraint.i->getVelocity(), jac_i) + glm::dot(collisionConstraint.j->getVelocity(), jac_j);
	}
	else
	{
		return glm::dot(collisionConstraint.i->getVelocity(), jac_i);
	}
}

float ConstraintSolver::residualNorm2(vector<float> residual, const GLuint NUM_CONSTRAINTS)
{
	float norm = 0.0f;
	for (int i = 0; i < NUM_CONSTRAINTS; i++)
	{
		norm += residual[i] * residual[i];
	}

	return norm;
}