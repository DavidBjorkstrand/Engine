#include "engine/scene/entity/component/Component.h"

#include "engine/scene/entity/Entity.h"

Entity *Component::getEntity()
{
	return _entity;
}