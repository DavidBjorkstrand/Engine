#include "engine/scene/entity/component/RigidbodyComponent.h"

#include "engine/physics/Rigidbody.h"
#include "engine/scene/Scene.h"
#include "engine/scene/entity/Entity.h"
#include "engine/scene/entity/Transform.h"
#include "engine/physics/Collider.h"

RigidbodyComponent::RigidbodyComponent(float mass)
{
	float i = (2.0f / 5.0f)*100.0f*100.0f;
	glm::mat3 I = glm::mat3(0.0f);
	I[0].x = i;
	I[1].y = i;
	I[2].z = i;

	_rigidbody = new Rigidbody(glm::vec3(0.0f), mass, I);
	_rigidbody->addListener(this);
}

void RigidbodyComponent::accept(Scene *scene)
{
	scene->visit(this);
}

void RigidbodyComponent::onAttach()
{
	_rigidbody->setPosition(getEntity()->getTransform()->getWorldPosition());

	Collider *collider = nullptr;
	collider = getEntity()->getComponent<Collider>();

	if (collider != nullptr)
	{
		collider->setRigidBody(_rigidbody);
	}
}

void RigidbodyComponent::notify(glm::vec3 position)
{
	getEntity()->getTransform()->setPosition(position);
}

void RigidbodyComponent::notify(glm::quat rotation)
{
	getEntity()->getTransform()->setOrientation(rotation);
}

Rigidbody *RigidbodyComponent::getRigidbody()
{
	return _rigidbody;
}