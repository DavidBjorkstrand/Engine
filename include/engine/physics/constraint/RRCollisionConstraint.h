#pragma once

#include "Constraint.h"

#include <glm/gtc/type_ptr.hpp>

class Rigidbody;

class RRCollisionConstraint : public Constraint
{
	private:
		Rigidbody *_r1;
		Rigidbody *_r2;
		glm::vec3 _normal;
		float _overlap;

	public:
		RRCollisionConstraint(Rigidbody *r1, Rigidbody *r2, glm::vec3 normal, float overlap);
		float b(float dt, float d);
		float Gv();
		float Dii();
		void updateVelocity(float deltaLambda);
		bool complementaryCondition();
};