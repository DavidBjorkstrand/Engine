#pragma once

#include "Constraint.h"

#include <glm/gtc/type_ptr.hpp>

class Rigidbody;

class RCollisionConstraint : public Constraint
{
	private:
		Rigidbody *_rigidbody;
		glm::vec3 _normal;
		float _overlap;

	public:
		RCollisionConstraint(Rigidbody *rigidbody, glm::vec3 normal, float overlap);
		float b(float dt, float d);
		float Gv();
		float Dii();
		void updateVelocity(float deltaLambda);

};