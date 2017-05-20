#pragma once

#include "engine/scene/entity/component/Component.h"

#include <GL\glew.h>
#include <glm/gtc/type_ptr.hpp>

class Scene;
class SphereCollider;
class PlaneCollider;
struct Particle;

struct Intersection
{
	GLboolean intersecting;
	glm::vec3 point;
	glm::vec3 normal;
	float distance;
};

class Collider : public Component
{
	public:
		void accept(Scene *scene);
		virtual Intersection checkCollision(Particle *particle) = 0;
		virtual Intersection checkCollision(SphereCollider *sphereCollider) = 0;
		virtual Intersection checkCollision(PlaneCollider *sphereCollider) = 0;
};