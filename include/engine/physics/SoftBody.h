#pragma once

#include "engine/physics/ParticleSystem.h"

#include <vector>

#include <GL/glew.h>
#include <glm/gtc/type_ptr.hpp>

struct SpringConstraint;
struct PPDistanceConstraint;

class SoftBody : public ParticleSystem
{
	private:
		glm::vec3 _position;
		vector<SpringConstraint> *_springConstraints;
		vector<PPDistanceConstraint> *_distanceConstraints;

	public:
		SoftBody(glm::vec3 position, GLint numParticles, float particleMass);
		~SoftBody();
		void setPosition(glm::vec3 position);
		void addSpringConstraint(GLuint particleIndexI, GLuint particleIndexJ, float length, float ks, float kd);
		void addDistanceConstraint(GLuint particleIndexI, GLuint particleIndexJ, float length);
		vector<SpringConstraint> *getSpringConstraints();
		vector<PPDistanceConstraint> *getDistanceConstraints();
};