#include "engine/physics/PlaneCollider.h"

#include "engine/scene/entity/Entity.h"
#include "engine/scene/entity/Transform.h"
#include "engine/scene/entity/component/Component.h"
#include "engine/physics/PhysicsSystem.h"

#include <glm\gtc\type_ptr.hpp>

PlaneCollider::PlaneCollider(glm::vec3 position, float width, float height)
{
	glm::vec3 y = (position - glm::vec3(0.0f, height / 2.0f, 0.0f)) - (position + glm::vec3(0.0f, height / 2.0f, 0.0f));
	glm::vec3 x = (position - glm::vec3(width / 2.0f, 0.0f, 0.0f)) - (position + glm::vec3(width / 2.0f, 0.0f, 0.0f));

	_planeNormal = glm::normalize(glm::cross(x, y));
	_D = glm::dot(-_planeNormal, position);
}

void PlaneCollider::setWidth(float width)
{
	_width = width;
}

void PlaneCollider::setHeight(float height)
{
	_height = height;
}

Collision PlaneCollider::checkCollision(Particle *particle)
{
	Collision collision;
	_transform = getEntity()->getTransform();
	glm::vec3 planeNormal = glm::normalize(glm::mat3(_transform->getMatrix()) * _planeNormal);
	float distanceNominator = glm::dot(planeNormal, particle->position) + _D;
	float distanceDenominator = glm::length(planeNormal);
	float distance = distanceNominator / distanceDenominator;
	glm::vec3 planeProjection = glm::dot(_planeNormal, particle->position)*_planeNormal;
	float planeDistance = glm::length(planeProjection - glm::vec3(0.0f));

	if (distance > particle->radius)
	{
		collision.colliding = false;
	}
	else if (planeDistance < 50.0f)
	{
		collision.colliding = true;
		collision.distance = distance - particle->radius;
		collision.normal = -planeNormal;
		collision.point = particle->position + collision.normal*distance;
	}

	return collision;
}

Collision PlaneCollider::checkCollision(SphereCollider *sphereCollider)
{
	return Collision();
}

Collision PlaneCollider::checkCollision(PlaneCollider *planeCollider)
{
	return Collision();
}