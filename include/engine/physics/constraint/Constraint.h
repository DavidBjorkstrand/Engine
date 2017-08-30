#pragma once

class Constraint
{
	public:
		virtual float b(float dt, float d) = 0;
		virtual float Gv() = 0;
		virtual float Dii() = 0;
		virtual void updateVelocity(float deltaLambda) = 0;
		virtual bool complementaryCondition() = 0;
};