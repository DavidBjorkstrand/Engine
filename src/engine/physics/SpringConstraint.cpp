#include "engine/physics/SpringConstraint.h"

#include "engine/physics/ParticleSystem.h"

#include <glm/gtc/type_ptr.hpp>

SpringConstraint::SpringConstraint(Particle *particlei, Particle *particlej, float length, float  Ks, float Kd)
{
	_particlei = particlei;
	_particlej = particlej;
	_length = length;
	_Ks = Ks;
	_Kd = Kd;
}

void SpringConstraint::updateForces()
{
	glm::vec3 r_ij = _particlei->position - _particlej->position;
	glm::vec3 v_ij = _particlei->predictedVelocity - _particlej->predictedVelocity;
	glm::vec3 direction = glm::normalize(r_ij);
	float length = glm::length(r_ij);

	glm::vec3 f_i = -(_Ks*(length - _length) + _Kd*(glm::dot(v_ij, r_ij) / length))*direction;
	glm::vec3 f_j = -f_i;

	_particlei->force += f_i;
	_particlej->force += f_j;
}

