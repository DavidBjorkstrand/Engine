#include "engine/scene/entity/component/Behaviour.h"

#include "engine/scene/Scene.h"

void Behaviour::accept(Scene *scene)
{
	scene->visit(this);
}

void Behaviour::onAttach()
{

}