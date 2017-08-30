#pragma once

#include "engine/scene/entity/component/Component.h"

#include <GL\glew.h>
#include <glm/gtc/type_ptr.hpp>

class Scene;
class SphereCollider;
class PlaneCollider;
class Rigidbody;
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
	private:
		GLboolean _hasBody;
		Rigidbody *_rigidbody;

	public:
		void accept(Scene *scene);
		void onAttach();
		void setRigidBody(Rigidbody *rigidbody);
		GLboolean hasBody();
		Rigidbody *getRigidbody();
		virtual Intersection checkCollision(Particle *particle) = 0;
		virtual Intersection checkCollision(SphereCollider *sphereCollider) = 0;
		virtual Intersection checkCollision(PlaneCollider *sphereCollider) = 0;
};