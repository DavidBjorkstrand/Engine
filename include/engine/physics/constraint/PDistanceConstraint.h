#pragma once

#include "Constraint.h"

#include <glm/gtc/type_ptr.hpp>

struct Particle;

class PDistanceConstraint : public Constraint
{
private:
	Particle *_p;
	glm::vec3 _position;
	float _length;

public:
	PDistanceConstraint(Particle *p, glm::vec3 position, float length);
	float b(float dt, float d);
	float Gv();
	float Dii();
	void updateVelocity(float deltaLambda);
	bool complementaryCondition();

};