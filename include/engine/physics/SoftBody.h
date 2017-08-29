#pragma once

#include "engine/physics/ParticleSystem.h"

#include <vector>

#include <GL/glew.h>
#include <glm/gtc/type_ptr.hpp>

struct SpringConstraint;
class Constraint;

class SoftBody : public ParticleSystem
{
	private:
		glm::vec3 _position;
		vector<SpringConstraint> *_springConstraints;
		vector<Constraint *> *_constraints;

	public:
		SoftBody(glm::vec3 position, GLint numParticles, float particleMass);
		~SoftBody();
		void setPosition(glm::vec3 position);
		void addSpringConstraint(GLuint particleIndexI, GLuint particleIndexJ, float length, float ks, float kd);
		void addDistanceConstraint(GLuint particleIndex, glm::vec3 position, float length);
		void addDistanceConstraint(GLuint particleIndexI, GLuint particleIndexJ, float length);
		vector<SpringConstraint> *getSpringConstraints();
		vector<Constraint *> *getConstraints();
};