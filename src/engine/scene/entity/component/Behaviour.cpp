#include "engine/scene/entity/component/Behaviour.h"

#include "engine/scene/SceneParser.h"

void Behaviour::accept(SceneParser *sceneParser)
{
	sceneParser->visit(this);
}