#pragma once

#include "engine/physics/Collider.h"
#include "engine/scene/entity/component/Component.h"

class Scene;

class SphereCollider : public Collider
{
	private:
		float _radius;

	public:
		SphereCollider(float radius);
		void setRadius(float radius);
		Collision checkCollision(Particle *particle);
		Collision checkCollision(SphereCollider *sphereCollider);
		Collision checkCollision(PlaneCollider *sphereCollider);
};