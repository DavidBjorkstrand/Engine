#pragma once

#include "Constraint.h"

#include <glm/gtc/type_ptr.hpp>

class Rigidbody;
struct Particle;

class RPCollisionConstraint : public Constraint
{
private:
	Rigidbody *_r;
	Particle *_p;
	glm::vec3 _normal;
	float _overlap;

public:
	RPCollisionConstraint(Rigidbody *r, Particle *p, glm::vec3 normal, float overlap);
	float b(float dt, float d);
	float Gv();
	float Dii();
	void updateVelocity(float deltaLambda);
	bool complementaryCondition();
};