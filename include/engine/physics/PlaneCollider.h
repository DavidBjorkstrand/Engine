#pragma once

#include "engine/physics/Collider.h"
#include "engine/scene/entity/component/Component.h"

#include <glm\gtc\type_ptr.hpp>

class Scene;
class SphereCollider;
class Transform;
struct Particle;

class PlaneCollider : public Collider
{
	private:
		Transform *_transform;
		float _width;
		float _height;
		glm::vec3 _planeNormal;
		float _D;

	public:
		PlaneCollider(glm::vec3 position, float width, float height);
		void setWidth(float width);
		void setHeight(float height);
		Intersection checkCollision(Particle *particle);
		Intersection checkCollision(SphereCollider *sphereCollider);
		Intersection checkCollision(PlaneCollider *planeCollider);
};