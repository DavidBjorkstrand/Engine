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

Intersection SphereCollider::checkCollision(Particle *particle)
{
	Intersection intersection;
	glm::vec3 position = getEntity()->getTransform()->getWorldPosition();
	glm::vec3 normal = glm::normalize(position - particle->position);
	float distance = glm::length(position - particle->position);

	if ((distance - _radius - particle->radius) > 0.0f)
	{
		intersection.intersecting = false;
	}
	else
	{
		intersection.intersecting = true;
		intersection.point = particle->position + normal*(distance - _radius);
		intersection.normal = normal;
		intersection.distance = distance - _radius - particle->radius;
	}

	return intersection;
}

Intersection SphereCollider::checkCollision(SphereCollider *sphereCollider)
{
	return Intersection();
}

Intersection SphereCollider::checkCollision(PlaneCollider *planeCollider)
{
	return Intersection();
}