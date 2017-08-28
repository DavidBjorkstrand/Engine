#include "engine/physics/PlaneCollider.h"

#include "engine/scene/entity/Entity.h"
#include "engine/scene/entity/Transform.h"
#include "engine/scene/entity/component/Component.h"
#include "engine/physics/ParticleSystem.h"
#include "engine/physics/SphereCollider.h"

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

Intersection PlaneCollider::checkCollision(Particle *particle)
{
	Intersection intersection;
	_transform = getEntity()->getTransform();
	_D = glm::dot(-_planeNormal, _transform->getWorldPosition());
	glm::vec3 planeNormal = glm::normalize(glm::mat3(_transform->getMatrix()) * _planeNormal);
	float distanceNominator = glm::dot(planeNormal, particle->position) + _D;
	float distanceDenominator = glm::length(planeNormal);
	float distance = distanceNominator / distanceDenominator;


	if (distance > particle->radius)
	{
		intersection.intersecting = false;
	}
	else
	{
		intersection.intersecting = true;
		intersection.distance = distance - particle->radius;
		intersection.normal = -planeNormal;
		intersection.point = particle->position + intersection.normal*distance;
	}

	return intersection;
}

Intersection PlaneCollider::checkCollision(SphereCollider *sphereCollider)
{
	Intersection intersection;
	_transform = getEntity()->getTransform();
	_D = glm::dot(-_planeNormal, _transform->getWorldPosition());
	glm::vec3 planeNormal = glm::normalize(glm::mat3(_transform->getMatrix()) * _planeNormal);
	float distanceNominator = glm::dot(planeNormal, sphereCollider->getPosition()) + _D;
	float distanceDenominator = glm::length(planeNormal);
	float distance = distanceNominator / distanceDenominator;
	glm::vec3 planeProjection = glm::dot(_planeNormal, sphereCollider->getPosition())*_planeNormal;
	float planeDistance = glm::length(planeProjection - _transform->getWorldPosition());

	if (distance > sphereCollider->getRadius())
	{
		intersection.intersecting = false;
	}
	else
	{
		intersection.intersecting = true;
		intersection.distance = distance - sphereCollider->getRadius();
		intersection.normal = -planeNormal;
		intersection.point = sphereCollider->getPosition() + intersection.normal*distance;
	}

	return intersection;
}

Intersection PlaneCollider::checkCollision(PlaneCollider *planeCollider)
{
	return Intersection();
}