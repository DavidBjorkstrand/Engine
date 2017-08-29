#pragma once

#include "Constraint.h"

#include <glm/gtc/type_ptr.hpp>

struct Particle;

class PCollisionConstraint : public Constraint
{
private:
	Particle *_p;
	glm::vec3 _normal;
	float _overlap;

public:
	PCollisionConstraint(Particle *p, glm::vec3 normal, float overlap);
	float b(float dt, float d);
	float Gv();
	float Dii();
	void updateVelocity(float deltaLambda);
	bool complementaryCondition();

};