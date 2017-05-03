#pragma once

#include "engine/physics/ParticleSystem.h"

#include <vector>

#include <GL/glew.h>
#include <glm/gtc/type_ptr.hpp>

struct SpringConstraint;

class SoftBody : public ParticleSystem
{
	private:
		glm::vec3 _position;
		vector<SpringConstraint> *_constraints;

	public:
		SoftBody(glm::vec3 position, GLint numParticles, float particleMass);
		~SoftBody();
		void setPosition(glm::vec3 position);
		void addConstraint(GLuint particleIndexI, GLuint particleIndexJ, float length, float ks, float kd);
		vector<SpringConstraint> *getConstraints();
};