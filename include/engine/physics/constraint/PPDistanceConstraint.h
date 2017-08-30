#pragma once

#include "Constraint.h"

#include <glm/gtc/type_ptr.hpp>

struct Particle;

class PPDistanceConstraint : public Constraint
{
private:
	Particle *_p1;
	Particle *_p2;
	float _length;

public:
	PPDistanceConstraint(Particle *p, Particle *_p2, float length);
	float b(float dt, float d);
	float Gv();
	float Dii();
	void updateVelocity(float deltaLambda);
	bool complementaryCondition();

};