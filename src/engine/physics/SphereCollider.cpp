#include "engine/physics/SphereCollider.h"

#include "engine/scene/Scene.h"
#include "engine/scene/entity/Entity.h"
#include "engine/scene/entity/Transform.h"
#include "engine/physics/Collider.h"
#include "engine/physics/ParticleSystem.h"

#include <GL\glew.h>
#include <glm\gtc\type_ptr.hpp>

SphereCollider::SphereCollider(float radius)
{
	_radius = radius;
}

void SphereCollider::setRadius(float radius)
{
	_radius = radius;
}

Collision SphereCollider::checkCollision(Particle *particle)
{
	Collision collision;
	glm::vec3 position = getEntity()->getTransform()->getWorldPosition();
	glm::vec3 normal = glm::normalize(position - particle->position);
	float distance = glm::length(position - particle->position);

	if ((distance - _radius - particle->radius) > 0.0f)
	{
		collision.colliding = false;
	}
	else
	{
		collision.colliding = true;
		collision.point = particle->position + normal*(distance - _radius);
		collision.normal = normal;
		collision.distance = distance - _radius - particle->radius;
	}

	return collision;
}

Collision SphereCollider::checkCollision(SphereCollider *sphereCollider)
{
	return Collision();
}

Collision SphereCollider::checkCollision(PlaneCollider *sphereCollider)
{
	return Collision();
}