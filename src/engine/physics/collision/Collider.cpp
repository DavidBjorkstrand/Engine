#include "engine/physics/collision/Collider.h"

#include "engine/scene/Scene.h"
#include "engine/scene/entity/Entity.h"
#include "engine/scene/entity/component/RigidbodyComponent.h"
#include "engine/physics/Rigidbody.h"

void Collider::accept(Scene *scene)
{
	scene->visit(this);
}

void Collider::onAttach()
{
	RigidbodyComponent *rigidbodyCompoenent = nullptr; 
	rigidbodyCompoenent = getEntity()->getComponent<RigidbodyComponent>();

	if (rigidbodyCompoenent != nullptr)
	{
		_hasBody = true;
		_rigidbody = rigidbodyCompoenent->getRigidbody();
	}
}

void Collider::setRigidBody(Rigidbody *rigidbody)
{
	_rigidbody = rigidbody;
}

GLboolean Collider::hasBody()
{
	return _hasBody;
}

Rigidbody *Collider::getRigidbody()
{
	return _rigidbody;
}