#include "Behaviour.h"
#include "..\..\SceneParser.h"

void Behaviour::accept(SceneParser *sceneParser)
{
	sceneParser->visit(this);
}