#pragma once

struct Particle;

// Only works when particles doesn't change address (vector resizing)
class SpringConstraint
{
	private:
		Particle *_particlei;
		Particle *_particlej;
		float _length;
		float _Ks;
		float _Kd;

	public:
		SpringConstraint(Particle *particlei, Particle *particlej, float length, float Ks, float Kd);
		void updateForces();
};