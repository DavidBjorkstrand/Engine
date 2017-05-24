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
		float getRadius();
		glm::vec3 getPosition();
		Intersection checkCollision(Particle *particle);
		Intersection checkCollision(SphereCollider *sphereCollider);
		Intersection checkCollision(PlaneCollider *sphereCollider);
};