#include "engine/physics/Collider.h"

#include "engine/scene/Scene.h"

void Collider::accept(Scene *scene)
{
	scene->visit(this);
}